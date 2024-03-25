#define MAIN

#include <signal.h>

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
//  start >>= 8;
//  end >>= 8;
  if (typ == 'A') {
    ramStart = start;
    ramEnd = end;
    }
  if (typ == 'O') {
    romStart = start;
    romEnd = end;
    }
  }

void processOut(char* arg) {
  byte port;
  int  file;
  if (*arg < '1' || *arg > '7') return;
  port = *arg - '1';
  arg++;
  if (*arg != '=') return;
  arg++;
  if (strcmp(arg, "stdin") == 0 ) file = 0;
  else if (strcmp(arg, "stdout") == 0) file = 1;
  else if (strcmp(arg, "stderr") == 0) file = 2;
  else file = open(arg, O_CREAT | O_TRUNC | O_WRONLY, 0666);
  if (file < 0) {
    printf("Could not open file: %s\n",arg);
    return;
    }
  out[port] = file;
  strcpy(outNames[port], arg);
  }

void processInp(char* arg) {
  byte port;
  int  file;
  if (*arg < '1' || *arg > '7') return;
  port = *arg - '1';
  arg++;
  if (*arg != '=') return;
  arg++;
  if (strcmp(arg, "stdin") == 0 ) file = 0;
  else if (strcmp(arg, "stdout") == 0) file = 1;
  else if (strcmp(arg, "stderr") == 0) file = 2;
  else file = open(arg, O_RDONLY, 0666);
  if (file < 0) {
    printf("Could not open file: %s\n",arg);
    return;
    }
  inp[port] = file;
  strcpy(inpNames[port], arg);
  }

void processIO(char* arg) {
  byte port;
  int  file;
  if (*arg < '1' || *arg > '7') return;
  port = *arg - '1';
  arg++;
  if (*arg != '=') return;
  arg++;
  if (strcmp(arg, "stdin") == 0 ) file = 0;
  else if (strcmp(arg, "stdout") == 0) file = 1;
  else if (strcmp(arg, "stderr") == 0) file = 2;
  else file = open(arg, O_RDWR);
  if (file < 0) {
    printf("Could not open file: %s\n",arg);
    return;
    }
  inp[port] = file;
  out[port] = file;
  strcpy(inpNames[port], arg);
  strcpy(outNames[port], arg);
  }

void processArg(char* arg) {
  int f;
  word addr;
  char buffer[256];
  if (strcmp(arg,"-elfos") == 0) useElfos = 0xff;
  else if (strcmp(arg,"-e") == 0) useElfos = 0xff;
  else if (strcmp(arg,"-ne") == 0) useElfos = 0;
  else if (strcmp(arg,"-boot") == 0) bootdisk = 0xff;
  else if (strcmp(arg,"-noboot") == 0) bootdisk = 0;
  else if (strcmp(arg,"-B") == 0) bootdisk = 0xff;
  else if (strcmp(arg,"-nB") == 0) bootdisk = 0;
  else if (strcmp(arg,"-d") == 0) runDebugger = 0xff;
  else if (strcmp(arg,"-nd") == 0) runDebugger = 0;
  else if (strcmp(arg,"-1802") == 0) use1805 = 0;
  else if (strcmp(arg,"-1805") == 0) use1805 = 0xff;
  else if (strcmp(arg,"-4") == 0) { useElfos = 0xff; elfos4 = 0xff; }
  else if (strcmp(arg,"-t") == 0) showTrace = 0xff;
  else if (strcmp(arg,"-u") == 0) liveUpdate = 0xff;
  else if (strcmp(arg,"-V") == 0) {
    printf("Run/02 v%s\n",VERSION);
    printf("by Michael H. Riley\n");
    }
  else if (strcmp(arg,"-nu") == 0) liveUpdate = 0;
  else if (strcmp(arg,"-v") == 0) { useVisual = 0xff; runDebugger = 0xff; }
  else if (strcmp(arg,"-nv") == 0) useVisual = 0;
  else if (strcmp(arg,"-nb") == 0) useBios = 0;
  else if (strcmp(arg,"-f800") == 0) useF800 = 0xff;
  else if (strcmp(arg,"-nf800") == 0) useF800 = 0;
  else if (strcmp(arg,"-rtc") == 0) useRTC = 0xff;
  else if (strcmp(arg,"-nrtc") == 0) useRTC = 0;
  else if (strcmp(arg,"-uart") == 0) useUART = 0xff;
  else if (strcmp(arg,"-nuart") == 0) useUART = 0;
  else if (strcmp(arg,"-nvr") == 0) useNVR = 0xff;
  else if (strcmp(arg,"-nnvr") == 0) useNVR = 0;
  else if (strncmp(arg,"-ram=",5) == 0) processMem(arg+5,'A');
  else if (strncmp(arg,"-rom=",5) == 0) processMem(arg+5,'O');
  else if (strncmp(arg,"-none=",6) == 0) processMem(arg+6,'X');
  else if (strncmp(arg,"-exec=",6) == 0) execAddr = getHex(arg+6);
  else if (strncmp(arg,"-o",2) == 0) processOut(arg+2);
  else if (strncmp(arg,"-io",3) == 0) processIO(arg+3);
  else if (strncmp(arg,"-i",2) == 0) processInp(arg+2);
  else if (strncmp(arg,"-c=",3) == 0) {
    freq = 125000 * atof(arg+3);
    freq /= 100;
printf("Cycles per 10ms: %d\n",freq);
    }
  else if (strncmp(arg,"-a=",3) == 0) {
    strcpy(args, arg+3);
    printf("Args: %s\n",args);
    }
  else if (strncmp(arg,"-b=",3) == 0) {
    f = open(arg+3, O_RDONLY);
    if (f <= 0) {
      printf("Could not open binary file: %s\n",arg);
      exit(1);
      }
    read(f, buffer, 6);
    addr = (buffer[0] << 8) | buffer[1];
    size = (buffer[2] << 8) | buffer[3];
    exec = (buffer[4] << 8) | buffer[5];
    read(f, &(cpu.ram[addr]), size);
    close(f);
    }
  else loader(arg);
  }

void controlC(int i) {
printf("Control c pressed\n"); fflush(stdout);
  ctrlC = -1;
  }

int main(int argc, char** argv) {
  int i;
  FILE* file;
  int f;
  char buffer[256];
  struct timeval startTime;
  struct timeval endTime;
  struct timeval tv;
  long long st,et;
  long long pst;
  long long pet;
  struct termios terminal;
  for (i=0; i<256; i++) imap[i] = 0;
  ctrlC = 0;
  use1805 = 0;
  useElfos = 0;
  useBios = 0xff;
  useConditions = 0xff;
  useVisual = 0;
  useF800 = 0xff;
  useRTC = 0;
  useUART = 0;
  useNVR = 0;
  liveUpdate = 0;
  runDebugger = 0;
  elfos4 = 0;
  exec = 0xffff;
  execAddr = 0x0000;
  icount = 0;
  freq = 0;
  bootdisk = 0;
  showTrace = 0;
  showMap = 0;
  strcpy(args,"");
  numConditions = 0;
  ramStart = 0x0000;
  ramEnd = 0xf7ff;
  romStart = 0xffff;
  romEnd = 0xffff;
  for (i=0; i<8; i++) {
    inp[i] = i;
    out[i] = i;
    strcpy(inpNames[i],"");
    strcpy(outNames[i],"");
    }
  strcpy(inpNames[0],"stdin");
  strcpy(outNames[1],"stdout");
  strcpy(outNames[2],"stderr");

  file = fopen("run02.rc","r");
  if (file != NULL) {
    while (fgets(buffer,255,file) != NULL) {
      while (strlen(buffer) > 0 && buffer[strlen(buffer)-1] <= ' ')
        buffer[strlen(buffer)-1] = 0;
      if (strlen(buffer) > 0) processArg(buffer);
      }
    fclose(file);
    }

  i = 1;
  while (i < argc) {
    processArg(argv[i]);
    i++;
    }
  
//  signal(SIGINT, controlC);

  for (i=0; i<256; i++) mmap[i] = 'X';
  if (useBios) {
    romStart = 0xf800;
    romEnd = 0xffff;
    }
  if (romStart != 0xffff) {
    for (i=romStart>>8; i<=romEnd>>8; i++) mmap[i] = 'O';
    }
  for (i=ramStart>>8; i<=ramEnd>>8; i++) mmap[i] = 'A';

  if (bootdisk != 0) execAddr = 0xff00;

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
    if (strlen(args) > 0) {
      for (i=0; i<strlen(args); i++) {
        cpu.ram[0x80+i] = args[i];
        cpu.ram[0x81+i] = 0;
        }
      }
    }

  if (useNVR) {
    f = open("run02.nvr", O_RDONLY);
    if (f > 0) {
      read(f, nvr, 128);
      close(f);
      }
    else {
      for (i=0; i<128; i++) nvr[i] = 0x00;
      }
    }

  printf("\n");

  if (runDebugger == 0) {
    tcgetattr(0,&terminal);
    original = terminal;
    terminal.c_lflag &= ~(ICANON | ISIG);
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
  if (freq) {
    periodCycles = 0;
    pst = startTime.tv_sec * 1000 + (tv.tv_usec / 1000);
    }
  if (runDebugger) {
    debugger(&cpu);
    }
  else {
      while (runFlag) {
      cycles = 0;
      cpuCycle(&cpu);
      periodCycles += cycles;
      if (cpu.idle) runFlag = 0;
      if (freq && runFlag) {
        if (periodCycles >= freq) {
          gettimeofday(&tv, NULL);
          pet = pst;
          while (pet-pst < 10) {
            gettimeofday(&tv, NULL);
            pet = tv.tv_sec * 1000 + (tv.tv_usec / 1000);
            }
          periodCycles = 0;
          pst = pet;
          }
        }
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
  for (i=0; i<8; i++) {
    if (out[i] > 2) close(out[i]);
    if (inp[i] > 2) close(inp[i]);
    }
  return 0;
  }

