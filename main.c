#define MAIN

#include "header.h"

word getHex(char* line) {
  word ret;
  ret = 0;
  while ((*line >= '0' && *line <= '9') ||
         (*line >= 'a' && *line <= 'f') ||
         (*line >= 'A' && *line <= 'F')) {
    if (*line >= '0' && *line <= '9') ret = (ret << 4) | (*line - '0');
    if (*line >= 'a' && *line <= 'f') ret = (ret << 4) | (*line - 87);
    if (*line >= 'A' && *line <= 'F') ret = (ret << 4) | (*line - 55);
    line++;
    }
  return ret;
  }

void processMem(char* line, char typ) {
  word i;
  word start;
  word end;
  byte which;
  char* orig;
  word n;
  orig = line;
  start = 0;
  end = 0;
  which = 0;
  while ((*line >= '0' && *line <= '9') ||
         (*line >= 'a' && *line <= 'f') ||
         (*line >= 'A' && *line <= 'F')) {
    n = 0xff;
    if (*line >= '0' && *line <= '9') n = *line - '0';
    if (*line >= 'a' && *line <= 'f') n = *line - 87;
    if (*line >= 'A' && *line <= 'F') n = *line - 55;
    if (n != 0xff) {
      if (which == 0) start = (start << 4) | n;
        else end = (end << 4) | n;
      }
    else {
      printf("Invalid memory specifiation.  Aborting: %s\n",orig);
      exit(1);
      }
    line++;
    if (*line == '-') {
      which = 1;
      line++;
      }
    }
  if (which == 0) {
    printf("Invalid memory specifiation.  Aborting: %s\n",orig);
    exit(1);
    }
  start >>= 8;
  end >>= 8;
  for (i=start; i<=end; i++) mmap[i] = typ;
  }

int main(int argc, char** argv) {
  int i;
  int f;
  word addr;
  word size;
  word exec;
  word execAddr;
  struct timeval startTime;
  struct timeval endTime;
  long long st,et;
  int  args;
  char buffer[256];
  struct termios terminal;
  for (i=0; i<256; i++) imap[i] = 0;
  use1805 = 0;
  useElfos = 0;
  useBios = 0xff;
  useConditions = 0xff;
  useVisual = 0;
  liveUpdate = 0;
  runDebugger = 0;
  elfos4 = 0;
  exec = 0xffff;
  execAddr = 0x0000;
  icount = 0;
  freq = 0;
  showTrace = 0;
  showMap = 0;
  args = -1;
  numConditions = 0;
  ramStart = 0x0000;
  ramEnd = 0xefff;
  for (i=0; i<256; i++) mmap[i] = 'A';
  if (useBios) {
    for (i=0xf8; i<=0xff; i++) mmap[i] = 'O';
    }
  i = 1;
  while (i < argc) {
    if (strcmp(argv[i],"-elfos") == 0) useElfos = 0xff;
    else if (strcmp(argv[i],"-e") == 0) useElfos = 0xff;
    else if (strcmp(argv[i],"-boot") == 0) execAddr = 0xff00;
    else if (strcmp(argv[i],"-B") == 0) execAddr = 0xff00;
    else if (strcmp(argv[i],"-d") == 0) runDebugger = 0xff;
    else if (strcmp(argv[i],"-1805") == 0) use1805 = 0xff;
    else if (strcmp(argv[i],"-4") == 0) { useElfos = 0xff; elfos4 = 0xff; }
    else if (strcmp(argv[i],"-t") == 0) showTrace = 0xff;
    else if (strcmp(argv[i],"-u") == 0) liveUpdate = 0xff;
    else if (strcmp(argv[i],"-v") == 0) { useVisual = 0xff; runDebugger = 0xff; }
    else if (strcmp(argv[i],"-nb") == 0) useBios = 0;
    else if (strncmp(argv[i],"-ram=",5) == 0) processMem(argv[i]+5,'A');
    else if (strncmp(argv[i],"-rom=",5) == 0) processMem(argv[i]+5,'O');
    else if (strncmp(argv[i],"-none=",6) == 0) processMem(argv[i]+6,'X');
    else if (strncmp(argv[i],"-exec=",6) == 0) execAddr = getHex(argv[i]+6);
    else if (strcmp(argv[i],"-c") == 0) {
      i++;
      freq = atof(argv[i]);
      freq = 1/(freq/8);
      }
    else if (strcmp(argv[i],"-a") == 0) {
      i++;
      args = i;
      printf("Args: %s\n",argv[i]);
      }
    else if (strcmp(argv[i],"-b") == 0) {
      i++;
      f = open(argv[i], O_RDONLY);
      if (f <= 0) {
        printf("Could not open binary file: %s\n",argv[i]);
        exit(1);
        }
      read(f, buffer, 6);
      addr = (buffer[0] << 8) | buffer[1];
      size = (buffer[2] << 8) | buffer[3];
      exec = (buffer[4] << 8) | buffer[5];
      read(f, &(cpu.ram[addr]), size);
      close(f);
      }
    else loader(argv[i]);
    i++;
    }

  if (useElfos) {
    cpu.ram[0x442] = (ramEnd & 0xff00) >> 8;
    cpu.ram[0x443] = (ramEnd & 0x00ff);
    cpu.ram[0x300] = 0x00;
    cpu.ram[0x303] = 0x00;
    cpu.ram[0x330] = 0xc0; cpu.ram[0x331] = 0xff; cpu.ram[0x332] = 0x03;
    cpu.ram[0x333] = 0xc0; cpu.ram[0x334] = 0xff; cpu.ram[0x335] = 0x09;
    cpu.ram[0x336] = 0xc0; cpu.ram[0x337] = 0xff; cpu.ram[0x338] = 0x06;
    cpu.ram[0x339] = 0xc0; cpu.ram[0x33a] = 0xff; cpu.ram[0x33b] = 0x0f;
    cpu.ram[0x34b] = 0xc0; cpu.ram[0x34c] = 0xff; cpu.ram[0x34d] = 0x66;
    cpu.ram[0x357] = 0xc0; cpu.ram[0x358] = 0xff; cpu.ram[0x359] = 0x69;
    cpu.ram[0x35d] = 0xc0; cpu.ram[0x35e] = 0xff; cpu.ram[0x35f] = 0x4e;
    cpu.ram[0x444] = 0xc0; cpu.ram[0x445] = 0xff; cpu.ram[0x446] = 0x03;
    cpu.ram[0x447] = 0xc0; cpu.ram[0x448] = 0xff; cpu.ram[0x449] = 0x09;
    cpu.ram[0x454] = 0xc0; cpu.ram[0x455] = 0xff; cpu.ram[0x456] = 0x06;
    cpu.ram[0x457] = 0xc0; cpu.ram[0x458] = 0xff; cpu.ram[0x459] = 0x0f;
    cpu.ram[0x80] = 0;
    if (elfos4) {
      cpu.ram[0x468] = (ramEnd - 255) >> 8;
      cpu.ram[0x469] = (ramEnd - 255);
      cpu.ram[0x442] = ((ramEnd-256) & 0xff00) >> 8;
      cpu.ram[0x443] = ((ramEnd-256) & 0x00ff);
      cpu.ram[0x465] = 0x40;
      cpu.ram[0x466] = 0x00;
      cpu.ram[ramEnd - 255] = 0x06;
      cpu.ram[ramEnd - 254] = 0x00;
      cpu.ram[ramEnd - 253] = 0xfc;
      cpu.ram[ramEnd] = 0;
      }
    if (args >= 0) {
      for (i=0; i<strlen(argv[args]); i++) {
        cpu.ram[0x80+i] = argv[args][i];
        cpu.ram[0x81+i] = 0;
        }
      }
    }

  printf("\n");

  if (runDebugger == 0) {
    tcgetattr(0,&terminal);
    original = terminal;
    terminal.c_lflag &= ~ICANON;
    terminal.c_lflag &= ~ECHO;
    if (tcsetattr(0,TCSANOW,&terminal) != 0) {
      printf("Could not set terminal attributes\n");
      exit(1);
      }
    }

  runFlag = 0xff;
  cpuReset(&cpu);
  cpu.r[0xe] = 0x0700;
  if (useElfos && exec != 0xffff) {
    cpu.p = 3;
    cpu.x = 2;
    cpu.r[3] = exec;
    cpu.r[6] = 0x300;
    cpu.r[4] = 0xfa7b;
    cpu.r[5] = 0xfa8d;
    cpu.r[2] = 0x1ffd;
    if (elfos4) {
      cpu.r[2] = ramEnd - 4;
      }
    cpu.r[0xa] = 0x80;
    }
  else cpu.r[0] = execAddr;
  gettimeofday(&startTime, NULL);
  if (runDebugger) {
    debugger(&cpu);
    }
  else {
      while (runFlag) {
      cpuCycle(&cpu);
      if (cpu.idle) runFlag = 0;
      }
    }
  gettimeofday(&endTime, NULL);
  if (runDebugger == 0) {
    if (tcsetattr(0,TCSANOW,&original) != 0) {
      printf("Could not restore terminal attributes\n");
      }
    }

  st = startTime.tv_sec * 1000000 + startTime.tv_usec;
  et = endTime.tv_sec * 1000000 + endTime.tv_usec;
  printf("\n");
  printf("Instructions executed: %lld\n",icount);
  printf("Run time             : %f\n",(et-st)/1000000.0);
  printf("Instructions/second  : %f\n",icount/((et-st)/1000000.0));

  if (showMap) {
    printf("     0  1  2  3  4  5  6  7    8  9  A  B  C  D  E  F\n");
    printf("0   ");
    for (i=0; i<256; i++) {
      if (imap[i] > 0) printf(" * ");
        else printf (" . ");
      if ((i+1)%8 == 0) printf("  ");
      if ((i+1)%16 == 0) printf("\n%X   ",(i+1)/16);
      }
    }
  return 0;
  }

