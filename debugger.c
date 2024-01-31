#include "header.h"

  char tmp[256];

word d_address;
word breakpoints[1024];
int  numBreakpoints;
byte traps[256];
char display[16][80];
char dline[256];

void output(char* line) {
  int i;
  for (i=0; i<15; i++)
    strcpy(display[i],display[i+1]);
  strcpy(display[15], line);
  for (i=0; i<16; i++) {
    printf("\e[%d;14H",i+5);
    printf("%-66s",display[i]);
    }
  }

void showScreen(CPU* cpu) {
  int i;
  printf("\e[1;1H");
  printf("+---------+\n");
  for (i=0; i<16; i++)
    printf("| R%X %04X |\n",i,cpu->r[i]);
  printf("+---------+\n");
  printf("\e[1;12H");
  printf(" +------+ +------+ +-----+ +-----+ +------+ +-----+ +------+\n");
  printf("\e[2;13H");
  printf("| D %02X | | DF %X | | X %X | | P %X | | T %02X | | Q %X | | IE %X |\n",
    cpu->d, cpu->df, cpu->x, cpu->p, cpu->t, cpu->q, cpu->ie);
  printf("\e[3;12H");
  printf(" +------+ +------+ +-----+ +-----+ +------+ +-----+ +------+\n");
  printf("\e[4;13H");
  printf("+------------------------------------------------------------------+\n");
  for (i=0; i<16; i++) {
    printf("\e[%d;13H",i+5);
    printf("|                                                                  |\n");
    }
  printf("\e[21;13H");
  printf("+------------------------------------------------------------------+\n");
  printf("\e[22;1H");
  }

void updateScreen(CPU* cpu) {
  int i;
  printf("\e[1;1H");
  printf("+---------+\n");
  for (i=0; i<16; i++)
    printf("| R%X %04X |\n",i,cpu->r[i]);
  printf("+---------+\n");
  printf("\e[1;12H");
  printf(" +------+ +------+ +-----+ +-----+ +------+ +-----+ +------+\n");
  printf("\e[2;13H");
  printf("| D %02X | | DF %X | | X %X | | P %X | | T %02X | | Q %X | | IE %X |\n",
    cpu->d, cpu->df, cpu->x, cpu->p, cpu->t, cpu->q, cpu->ie);
  printf("\e[3;12H");
  printf(" +------+ +------+ +-----+ +-----+ +------+ +-----+ +------+\n");
  printf("\e[1;75H");
  if (useConditions) printf("CND"); else printf("   ");
  printf("\e[23;1H");
  printf("%79s"," ");
  printf("\e[22;1H");
  }

word disassem1805(CPU* cpu, word address) {
  byte i;
  byte n;
  byte inst;
  word adr;
  inst = cpu->ram[address++];
  i = inst & 0xf0;
  n = inst & 0x0f;
  switch (i) {
    case 0x00:
         switch (n) {
           case 0x00:
                sprintf(tmp,"68 %02x       STPC",inst);
                break;
           case 0x01:
                sprintf(tmp,"68 %02x       DTC",inst);
                break;
           case 0x02:
                sprintf(tmp,"68 %02x       SPM2",inst);
                break;
           case 0x03:
                sprintf(tmp,"68 %02x       SCM2",inst);
                break;
           case 0x04:
                sprintf(tmp,"68 %02x       SPM1",inst);
                break;
           case 0x05:
                sprintf(tmp,"68 %02x       SCM1",inst);
                break;
           case 0x06:
                sprintf(tmp,"68 %02x       LDC",inst);
                break;
           case 0x07:
                sprintf(tmp,"68 %02x       STM",inst);
                break;
           case 0x08:
                sprintf(tmp,"68 %02x       GEC",inst);
                break;
           case 0x09:
                sprintf(tmp,"68 %02x       ETQ",inst);
                break;
           case 0x0a:
                sprintf(tmp,"68 %02x       XIE",inst);
                break;
           case 0x0b:
                sprintf(tmp,"68 %02x       XID",inst);
                break;
           case 0x0c:
                sprintf(tmp,"68 %02x       CIE",inst);
                break;
           case 0x0d:
                sprintf(tmp,"68 %02x       CID",inst);
                break;
           }
         break;
    case 0x20:
         adr = cpu->ram[address++] << 8;
         adr |= cpu->ram[address++];
         sprintf(tmp,"68 %02x %02x %02x DBNZ   R%X,%04X",inst,(adr & 0xff00)>>8,adr&0xff,n,adr);
         break;
    case 0x30:
         switch (n) {
           case 0x0e:
                adr = cpu->ram[address++];
                sprintf(tmp,"68 %02x %02x    BCI    %02X",inst,adr&0xff,adr&0xff);
                break;
           case 0x0f:
                adr = cpu->ram[address++];
                sprintf(tmp,"68 %02x %02x    BXI    %02X",inst,adr&0xff,adr&0xff);
                break;
           }
    case 0x60:
         sprintf(tmp,"68 %02x       RLXA   R%X",inst,n);
         break;
    case 0x70:
         switch (n) {
           case 0x04:
                sprintf(tmp,"68 %02x       DADC",inst);
                break;
           case 0x06:
                sprintf(tmp,"68 %02x       DSAV",inst);
                break;
           case 0x07:
                sprintf(tmp,"68 %02x       DSMB",inst);
                break;
           case 0x0c:
                adr = cpu->ram[address++];
                sprintf(tmp,"68 %02x %02x    DACI   %02X",inst,adr&0xff,adr&0xff);
                break;
           case 0x0f:
                adr = cpu->ram[address++];
                sprintf(tmp,"68 %02x %02x    DSBI   %02X",inst,adr&0xff,adr&0xff);
                break;
           }
         break;
    case 0x80:
         adr = cpu->ram[address++] << 8;
         adr |= cpu->ram[address++];
         sprintf(tmp,"68 %02x %02x %02x SCAL   R%X,%04X",inst,(adr & 0xff00)>>8,adr&0xff,n,adr);
         break;
    case 0x90:
         sprintf(tmp,"68 %02x       SRET   R%X",inst,n);
         break;
    case 0xa0:
         sprintf(tmp,"68 %02x       RSXD   R%X",inst,n);
         break;
    case 0xb0:
         sprintf(tmp,"68 %02x       RNX    R%X",inst,n);
         break;
    case 0xc0:
         adr = cpu->ram[address++] << 8;
         adr |= cpu->ram[address++];
         sprintf(tmp,"68 %02x %02x %02x RLDI   R%X,%04X",inst,(adr & 0xff00)>>8,adr&0xff,n,adr);
         break;
    case 0xf0:
         switch (n) {
           case 0x04:
                sprintf(tmp,"68 %02x       DADD",inst);
                break;
           case 0x07:
                sprintf(tmp,"68 %02x       DSM",inst);
                break;
           case 0x0c:
                adr = cpu->ram[address++];
                sprintf(tmp,"68 %02x %02x    DADI   %02X",inst,adr&0xff,adr&0xff);
                break;
           case 0x0f:
                adr = cpu->ram[address++];
                sprintf(tmp,"68 %02x %02x    DSMI   %02X",inst,adr&0xff,adr&0xff);
                break;
           }
         break;
    }
  return address;
  }

word disassem(CPU* cpu, word address) {
  byte i;
  byte n;
  byte inst;
  word adr;
  sprintf(dline, "%04x: ",address);
  inst = cpu->ram[address++];
  i = inst & 0xf0;
  n = inst & 0x0f;
  switch (i) {
    case 0x00:
         if (n == 0)
           sprintf(tmp, "00          IDL");
         else
           sprintf(tmp, "%02x          LDN    %X",inst,n);
         break;
    case 0x10:
         sprintf(tmp, "%02x          INC    %X",inst,n);
         break;
    case 0x20:
         sprintf(tmp, "%02x          DEC    %X",inst,n);
         break;
    case 0x30:
         adr = cpu->ram[address++];
         switch (n) {
           case 0x00:
                sprintf(tmp, "%02x %02x       BR     %02X",inst,adr,adr);
                break;
           case 0x01:
                sprintf(tmp, "%02x %02x       BQ     %02X",inst,adr,adr);
                break;
           case 0x02:
                sprintf(tmp, "%02x %02x       BZ     %02X",inst,adr,adr);
                break;
           case 0x03:
                sprintf(tmp, "%02x %02x       BDF    %02X",inst,adr,adr);
                break;
           case 0x04:
                sprintf(tmp, "%02x %02x       B1     %02X",inst,adr,adr);
                break;
           case 0x05:
                sprintf(tmp, "%02x %02x       B2     %02X",inst,adr,adr);
                break;
           case 0x06:
                sprintf(tmp, "%02x %02x       B3     %02X",inst,adr,adr);
                break;
           case 0x07:
                sprintf(tmp, "%02x %02x       B4     %02X",inst,adr,adr);
                break;
           case 0x08:
                sprintf(tmp, "%02x          SKP",inst);
                address--;
                break;
           case 0x09:
                sprintf(tmp, "%02x %02x       BNQ    %02X",inst,adr,adr);
                break;
           case 0x0a:
                sprintf(tmp, "%02x %02x       BNZ    %02X",inst,adr,adr);
                break;
           case 0x0b:
                sprintf(tmp, "%02x %02x       BNF    %02X",inst,adr,adr);
                break;
           case 0x0c:
                sprintf(tmp, "%02x %02x       BN1    %02X",inst,adr,adr);
                break;
           case 0x0d:
                sprintf(tmp, "%02x %02x       BN2    %02X",inst,adr,adr);
                break;
           case 0x0e:
                sprintf(tmp, "%02x %02x       BN3    %02X",inst,adr,adr);
                break;
           case 0x0f:
                sprintf(tmp, "%02x %02x       BN4    %02X",inst,adr,adr);
                break;
           }
         break;
    case 0x40:
         sprintf(tmp, "%02x          LDA    %X",inst,n);
         break;
    case 0x50:
         sprintf(tmp, "%02x          STR    %X",inst,n);
         break;
    case 0x60:
         switch (n) {
           case 0x00:
                sprintf(tmp, "%02x          IRX",inst);
                break;
           case 0x01:
                sprintf(tmp, "%02x          OUT    1",inst);
                break;
           case 0x02:
                sprintf(tmp, "%02x          OUT    2",inst);
                break;
           case 0x03:
                sprintf(tmp, "%02x          OUT    3",inst);
                break;
           case 0x04:
                sprintf(tmp, "%02x          OUT    4",inst);
                break;
           case 0x05:
                sprintf(tmp, "%02x          OUT    5",inst);
                break;
           case 0x06:
                sprintf(tmp, "%02x          OUT    6",inst);
                break;
           case 0x07:
                sprintf(tmp, "%02x          OUT    7",inst);
                break;
           case 0x08:
                address = disassem1805(cpu, address);
                break;
           case 0x09:
                sprintf(tmp, "%02x          INP    1",inst);
                break;
           case 0x0a:
                sprintf(tmp, "%02x          INP    2",inst);
                break;
           case 0x0b:
                sprintf(tmp, "%02x          INP    3",inst);
                break;
           case 0x0c:
                sprintf(tmp, "%02x          INP    4",inst);
                break;
           case 0x0d:
                sprintf(tmp, "%02x          INP    5",inst);
                break;
           case 0x0e:
                sprintf(tmp, "%02x          INP    6",inst);
                break;
           case 0x0f:
                sprintf(tmp, "%02x          INP    7",inst);
                break;
           }
         break;
    case 0x70:
         switch (n) {
           case 0x00:
                sprintf(tmp, "%02x          RET",inst);
                break;
           case 0x01:
                sprintf(tmp, "%02x          DIS",inst);
                break;
           case 0x02:
                sprintf(tmp, "%02x          LDXA",inst);
                break;
           case 0x03:
                sprintf(tmp, "%02x          STXD",inst);
                break;
           case 0x04:
                sprintf(tmp, "%02x          ADC",inst);
                break;
           case 0x05:
                sprintf(tmp, "%02x          SDB",inst);
                break;
           case 0x06:
                sprintf(tmp, "%02x          SHRC",inst);
                break;
           case 0x07:
                sprintf(tmp, "%02x          SMB",inst);
                break;
           case 0x08:
                sprintf(tmp, "%02x          SAV",inst);
                break;
           case 0x09:
                sprintf(tmp, "%02x          MARK",inst);
                break;
           case 0x0a:
                sprintf(tmp, "%02x          REQ",inst);
                break;
           case 0x0b:
                sprintf(tmp, "%02x          SEQ",inst);
                break;
           case 0x0c:
                adr = cpu->ram[address++];
                sprintf(tmp, "%02x %02x         ADCI   %02X",inst,adr,adr);
                break;
           case 0x0d:
                adr = cpu->ram[address++];
                sprintf(tmp, "%02x %02x         SDBI   %02X",inst,adr,adr);
                break;
           case 0x0e:
                sprintf(tmp, "%02x          SHLC",inst);
                break;
           case 0x0f:
                adr = cpu->ram[address++];
                sprintf(tmp, "%02x %02x         SMBI   %02X",inst,adr,adr);
                break;
           }
         break;
    case 0x80:
         sprintf(tmp, "%02x          GLO    %X",inst,n);
         break;
    case 0x90:
         sprintf(tmp, "%02x          GHI    %X",inst,n);
         break;
    case 0xa0:
         sprintf(tmp, "%02x          PLO    %X",inst,n);
         break;
    case 0xb0:
         sprintf(tmp, "%02x          PHI    %X",inst,n);
         break;
    case 0xc0:
         adr = cpu->ram[address++] << 8;
         adr |= cpu->ram[address++];
         switch (n) {
           case 0x00:
                sprintf(tmp, "%02x %02x %02x    LBR    %04X",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x01:
                sprintf(tmp, "%02x %02x %02x    LBQ    %04X",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x02:
                sprintf(tmp, "%02x %02x %02x    LBZ    %04X",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x03:
                sprintf(tmp, "%02x %02x %02x    LBDF   %04X",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x04:
                sprintf(tmp, "%02x            NOP",inst);
                address -= 2;
                break;
           case 0x05:
                sprintf(tmp, "%02x            LSNQ",inst);
                address -= 2;
                break;
           case 0x06:
                sprintf(tmp, "%02x            LSNZ",inst);
                address -= 2;
                break;
           case 0x07:
                sprintf(tmp, "%02x            LSNF",inst);
                address -= 2;
                break;
           case 0x08:
                sprintf(tmp, "%02x            LSKP",inst);
                address -= 2;
                break;
           case 0x09:
                sprintf(tmp, "%02x %02x %02x    LBNQ   %04X",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x0a:
                sprintf(tmp, "%02x %02x %02x    LBNZ   %04X",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x0b:
                sprintf(tmp, "%02x %02x %02x    LBNF   %04X",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x0c:
                sprintf(tmp, "%02x            LSIE",inst);
                address -= 2;
                break;
           case 0x0d:
                sprintf(tmp, "%02x            LSQ",inst);
                address -= 2;
                break;
           case 0x0e:
                sprintf(tmp, "%02x            LSZ",inst);
                address -= 2;
                break;
           case 0x0f:
                sprintf(tmp, "%02x            LSDF",inst);
                address -= 2;
                break;
           }
         break;
    case 0xd0:
         sprintf(tmp, "%02x          SEP    %X",inst,n);
         break;
    case 0xe0:
         sprintf(tmp, "%02x          SEX    %X",inst,n);
         break;
    case 0xf0:
         switch (n) {
           case 0x00:
                sprintf(tmp, "%02x          LDX",inst);
                break;
           case 0x01:
                sprintf(tmp, "%02x          OR",inst);
                break;
           case 0x02:
                sprintf(tmp, "%02x          AND",inst);
                break;
           case 0x03:
                sprintf(tmp, "%02x          XOR",inst);
                break;
           case 0x04:
                sprintf(tmp, "%02x          ADD",inst);
                break;
           case 0x05:
                sprintf(tmp, "%02x          SD",inst);
                break;
           case 0x06:
                sprintf(tmp, "%02x          SHR",inst);
                break;
           case 0x07:
                sprintf(tmp, "%02x          SM",inst);
                break;
           case 0x08:
                adr = cpu->ram[address++];
                sprintf(tmp, "%02x %02x       LDI    %02X",inst,adr,adr);
                break;
           case 0x09:
                adr = cpu->ram[address++];
                sprintf(tmp, "%02x %02x       ORI    %02X",inst,adr,adr);
                break;
           case 0x0a:
                adr = cpu->ram[address++];
                sprintf(tmp, "%02x %02x       ANI    %02X",inst,adr,adr);
                break;
           case 0x0b:
                adr = cpu->ram[address++];
                sprintf(tmp, "%02x %02x       XRI    %02X",inst,adr,adr);
                break;
           case 0x0c:
                adr = cpu->ram[address++];
                sprintf(tmp, "%02x %02x       ADI    %02X",inst,adr,adr);
                break;
           case 0x0d:
                adr = cpu->ram[address++];
                sprintf(tmp, "%02x %02x       SDI    %02X",inst,adr,adr);
                break;
           case 0x0e:
                sprintf(tmp, "%02x          SHL",inst);
                break;
           case 0x0f:
                adr = cpu->ram[address++];
                sprintf(tmp, "%02x %02x       SMI    %02X",inst,adr,adr);
                break;
           }
         break;

    }
  strcat(dline, tmp);
  while (strlen(dline) < 50) strcat(dline," ");
  return address;
  }

char* hexToBin(char* buffer, word *ret) {
  *ret = 0;
  while ((*buffer >= '0' && *buffer <= '9') ||
         (*buffer >= 'a' && *buffer <= 'f') ||
         (*buffer >= 'A' && *buffer <= 'F')) {
    if (*buffer >= '0' && *buffer <= '9') *ret = (*ret << 4) | (*buffer - '0');
    if (*buffer >= 'a' && *buffer <= 'f') *ret = (*ret << 4) | (*buffer - 'a' + 10);
    if (*buffer >= 'A' && *buffer <= 'F') *ret = (*ret << 4) | (*buffer - 'A' + 10);
    buffer++;
    }
  return buffer;
  }

void dbgShowCpu(CPU* cpu) {
  int x;
  char buffer[80];
  if (useVisual) {
    output("");
    sprintf(buffer," R0: %04X   R1: %04X   R2: %04X   R3: %04X",
      cpu->r[0], cpu->r[1], cpu->r[2], cpu->r[3]);
    output(buffer);
    sprintf(buffer," R4: %04X   R5: %04X   R6: %04X   R7: %04X",
      cpu->r[4], cpu->r[5], cpu->r[6], cpu->r[7]);
    output(buffer);
    sprintf(buffer," R8: %04X   R9: %04X   RA: %04X   RB: %04X",
      cpu->r[8], cpu->r[9], cpu->r[10], cpu->r[11]);
    output(buffer);
    sprintf(buffer," RC: %04X   RD: %04X   RE: %04X   RF: %04X",
      cpu->r[12], cpu->r[13], cpu->r[14], cpu->r[15]);
    output(buffer);
    sprintf(buffer,"  D: %4X   DF: %4X    P: %4X    X: %4X",
      cpu->d, cpu->df, cpu->p, cpu->x);
    output(buffer);
    sprintf(buffer,"  Q: %4X    T: %4X   IE: %4X",
      cpu->q, cpu->t, cpu->ie);
    output(buffer);
    if (use1805) {
      sprintf(buffer,"CTR: %4X   CH: %4X  CIE: %4X  XIE: %4X   CI: %4X",
        cpu->cntr, cpu->ch, cpu->cie, cpu->xie, cpu->ci);
      output(buffer);
      }
    }
  else {
    for (x=0; x<16; x++) {
      if (x==8) printf("\n");
      printf("   R%X:%04x",x,cpu->r[x]);
      }
    printf("\n");
    printf("    D:%02X     DF:%X       P:%X       X:%X       Q:%X",cpu->d, cpu->df, cpu->p, cpu->x, cpu->q);
    printf("       T:%02x     IE:%x\n",cpu->t, cpu->ie);
    if (use1805) {
      printf("  CTR:%02X     CH:%02X    CIE:%X     XIE:%X      CI:%X",cpu->cntr, cpu->ch, cpu->cie, cpu->xie, cpu->ci);
      printf("\n");
      }
    }
  }

void dbgShow(CPU* cpu, char* buffer) {
  int x,y;
  char line[80];
  char tmp[32];
  word a;
  if (*buffer == 'i' || *buffer == 'I') {
    output("Input ports:");
    for (x=0; x<7; x++) {
      sprintf(line,"  %d: %s",x+1,inpNames[x]);
      output(line);
      }
    return;
    }
  if (*buffer == 'o' || *buffer == 'O') {
    output("Output ports:");
    for (x=0; x<7; x++) {
      sprintf(line,"  %d: %s",x+1,outNames[x]);
      output(line);
      }
    return;
    }
  if (*buffer == 'm' || *buffer == 'M') {
    if (useVisual) {
      output("       01234567 89ABCDEF 01234567 89ABCDEF");
      output("       -------- -------- -------- --------");
      }
    else {
      printf("       01234567 89ABCDEF 01234567 89ABCDEF\n");
      printf("       -------- -------- -------- --------");
      }
    strcpy(line,"");
    for (x=0; x<256; x++) {
      if (useVisual) {
        if (x % 32 == 0) {
          if (strlen(line) > 0) output(line);
          sprintf(line,"%04x: ",x << 8);
          }
        if (x % 8 == 0) strcat(line, " ");
        sprintf(tmp,"%c",mmap[x]);
        strcat(line, tmp);
        }
      else {
        if (x % 32 == 0) printf("\n%04x: ",x << 8);
        if (x % 8 == 0) printf(" ");
        printf("%c",mmap[x]);
        }
      }
    if (useVisual) output(line);
      else printf("\n");
    return;
    }
  if (*buffer == '?') {
    dbgShowCpu(cpu);
    return;
    }
  if (*buffer == 'p' || *buffer == 'P') d_address = cpu->r[cpu->p];
  if (*buffer == 'x' || *buffer == 'X') d_address = cpu->r[cpu->x];
  if (*buffer == 'r' || *buffer == 'R') {
    hexToBin(buffer+1, &a);
    d_address = cpu->r[a & 0xf];
    }
  if ((*buffer >= '0' && *buffer <= '9') ||
      (*buffer >= 'a' && *buffer <= 'f') ||
      (*buffer >= 'A' && *buffer <= 'F'))
     buffer = hexToBin(buffer, &d_address);

  for (y=0; y<16; y++) {
    sprintf(line, "%04x:", d_address);
    for (x=0; x<16; x++) {
      sprintf(tmp, " %02x",cpu->ram[d_address+x]);
      strcat(line, tmp);
      }
    if (useVisual) output(line);
      else printf("%s\n",line);
    d_address += 16;
    }
  }

void dbgDisassem(CPU* cpu, char* buffer) {
  int i;
  word a;
  if (*buffer == 'p' || *buffer == 'P') d_address = cpu->r[cpu->p];
  if (*buffer == 'x' || *buffer == 'X') d_address = cpu->r[cpu->x];
  if (*buffer == 'r' || *buffer == 'R') {
    hexToBin(buffer+1, &a);
    d_address = cpu->r[a & 0xf];
    }
  if ((*buffer >= '0' && *buffer <= '9') ||
      (*buffer >= 'a' && *buffer <= 'f') ||
      (*buffer >= 'A' && *buffer <= 'F'))
     buffer = hexToBin(buffer, &d_address);

  for (i=0; i<16; i++) {
    d_address = disassem(cpu, d_address);
    if (useVisual) output(dline);
      else printf("%s\n",dline);
    }
  }

void dbgStore(CPU* cpu, char* buffer) {
  word addr;
  word b;
  byte f;
  if (*buffer == 'p' || *buffer == 'P') { addr = cpu->r[cpu->p]; buffer++; }
  if (*buffer == 'x' || *buffer == 'X') { addr = cpu->r[cpu->x]; buffer++; }
  if (*buffer == 'r' || *buffer == 'R') {
    buffer++;
    buffer = hexToBin(buffer, &addr);
    addr = cpu->r[addr & 0xf];
    }
  if ((*buffer >= '0' && *buffer <= '9') ||
      (*buffer >= 'a' && *buffer <= 'f') ||
      (*buffer >= 'A' && *buffer <= 'F'))
     buffer = hexToBin(buffer, &addr);

  f = 0xff;
  while (f) {
    while (*buffer == ' ') buffer++;
    if ((*buffer >= '0' && *buffer <= '9') ||
        (*buffer >= 'a' && *buffer <= 'f') ||
        (*buffer >= 'A' && *buffer <= 'F')) {
      buffer = hexToBin(buffer, &b);
      cpu->ram[addr++] = b & 0xff;
      }
    else f = 0;
    }
  }

void dbgPush(CPU* cpu, char* buffer) {
  word b;
  byte f;
  f = 0xff;
  while (f) {
    while (*buffer == ' ') buffer++;
    if ((*buffer >= '0' && *buffer <= '9') ||
        (*buffer >= 'a' && *buffer <= 'f') ||
        (*buffer >= 'A' && *buffer <= 'F')) {
      buffer = hexToBin(buffer, &b);
      cpu->ram[cpu->r[cpu->x]--] = b & 0xff;
      }
    else f = 0;
    }
  }

void dbgPop(CPU* cpu, char*buffer) {
  word a;
  hexToBin(buffer, &a);
  cpu->r[cpu->x] += a;
  }

void dbgCmdB(CPU* cpu, char* buffer) {
  int i;
  int j;
  word a;
  char line[80];
  char tmp[16];
  if (*buffer == '?') {
    if (useVisual) {
      output("");
      output("Break points:");
      strcpy(line,"");
      for (i=0; i<numBreakpoints; i++) {
        sprintf(tmp, "  %04x", breakpoints[i]);
        strcat(line, tmp);
        if (strlen(line) > 58) {
          output(line);
          strcpy(line,"");
          }
        }
      if (strlen(line) > 0) output(line);
      }
    else {
      printf("Break points:\n");
      for (i=0; i<numBreakpoints; i++)
        printf("  %04x\n",breakpoints[i]);
      printf("\n");
      }
    }
  else if (*buffer == '+') {
    buffer++;
    hexToBin(buffer, &a);
    breakpoints[numBreakpoints++] = a;
    }
  else if (*buffer == '-') {
    buffer++;
    hexToBin(buffer, &a);
    for (i=0; i<numBreakpoints; i++)
      if (breakpoints[i] == a) {
        for (j=i; j<numBreakpoints-1; j++)
          breakpoints[j] = breakpoints[j+1];
        numBreakpoints--;
        return;
        }
    }
  }

void dbgCmdC(CPU* cpu, char* buffer) {
  int i;
  int n;
  char tbuffer[256];
  if (strcasecmp(buffer,"ls") == 0) {
    if (useVisual) printf("\e[H\e[2J");
    if (useVisual) showScreen(cpu);
    return;
    }
  if (*buffer == '?') {
    if (useVisual) output("");
      else printf("\n");
    if (useVisual) output("Conditions:");
      else printf("Conditions:\n");
    for (i=0; i<numConditions; i++) {
      sprintf(tbuffer,"  %d: %s",i,conditions[i]);
      if (useVisual) output(tbuffer);
        else printf("%s\n",tbuffer);
      }
    return;
    }
  if (*buffer == '+') {
    buffer++;
    if (*buffer == '+') {
      useConditions = 0xff;
      return;
      }
    numConditions++;
    if (numConditions == 1)
      conditions = (char**)malloc(sizeof(char*));
    else
      conditions = (char**)realloc(conditions, sizeof(char*) * numConditions);
    conditions[numConditions-1] = (char*)malloc(strlen(buffer)+1);
    strcpy(conditions[numConditions-1], buffer);
    return;
    }
  if (*buffer == '-') {
    buffer++;
    if (*buffer == '-') {
      useConditions = 0;
      return;
      }
    n = atoi(buffer);
    if (n <0 || n>= numConditions) return;
    free(conditions[n]);
    for (i=n; i<numConditions-1; i++)
      conditions[i] = conditions[i+1];
    numConditions--;
    if (numConditions == 0) free(conditions);
    }
  }

void dbgCmdD(CPU* cpu, char* buffer) {
  word a;
  char line[80];
  if (*buffer == 'f' || *buffer == 'F') {
    buffer++;
    if (*buffer == '=') {
      buffer++;
      hexToBin(buffer, &a);
      cpu->df = a & 1;
      }
    else {
      sprintf(line, "DF=%d",cpu->df);
      if (useVisual) output(line);
        else printf("%s\n",line);
      }
    }
  else if (*buffer == 'i' || *buffer == 'I') {
    buffer++;
    while (*buffer == ' ') buffer++;
    hexToBin(buffer, &a);
    cpuDmaIn(cpu, a & 0xff);
    }
  else if (*buffer == 'o' || *buffer == 'O') {
    sprintf(line, "DMA OUT: %02x",cpuDmaOut(cpu));
    if (useVisual) output(line);
      else printf("%s\n",line);
    }
  else if (*buffer == '=') {
    buffer++;
    hexToBin(buffer, &a);
    cpu->d=a & 0xff;
    }
  else {
    sprintf(line, "D=%02X",cpu->d);
    if (useVisual) output(line);
      else printf("%s\n",line);
    }
  }

void dbgCmdI(CPU* cpu, char* buffer) {
  word a;
  char line[80];
  if (*buffer == 'e' || *buffer == 'E') {
    buffer++;
    if (*buffer == '=') {
      buffer++;
      hexToBin(buffer, &a);
      cpu->ie = a & 1;
      }
    else {
      sprintf(line, "IE=%X",cpu->ie);
      if (useVisual) output(line);
        else printf("%s\n",line);
      }
    }
  else cpuIntr(cpu,'E');
  }

void dbgCmdP(CPU* cpu, char* buffer) {
  word a;
  char line[80];
  if (*buffer == '=') {
    buffer++;
    hexToBin(buffer, &a);
    cpu->p=a & 0xf;
    }
  else {
    sprintf(line, "P=%X",cpu->p);
    if (useVisual) output(line);
      else printf("%s\n",line);
    }
  }

void dbgCmdQ(CPU* cpu, char* buffer) {
  word a;
  char line[80];
  if (*buffer == '=') {
    buffer++;
    hexToBin(buffer, &a);
    cpu->q=a & 0x1;
    }
  else {
    sprintf(line, "Q=%X",cpu->q);
    if (useVisual) output(line);
      else printf("%s\n",line);
    }
  }

void dbgCmdR(CPU* cpu, char* buffer) {
  word a;
  byte r;
  char line[80];
  if (*buffer == 0) {
    cpuReset(cpu);
    return;
    }
  buffer= hexToBin(buffer, &a);
  r = a & 0xf;
  if (*buffer == '=') {
    buffer++;
    hexToBin(buffer, &a);
    cpu->r[r] = a;
    }
  else {
    sprintf(line, "R%X=%X",r,cpu->r[r]);
    if (useVisual) output(line);
      else printf("%s\n",line);
    }
  }

void dbgCmdT(CPU* cpu, char* buffer) {
  int i;
  word a;
  char line[80];
  char tmp[16];
  if (*buffer == 'r' || *buffer == 'R') {
    buffer++;
    if (*buffer == '+') showTrace = 0xff;
    if (*buffer == '-') showTrace = 0x00;
    return;
    }
  if (*buffer == '?') {
    if (useVisual) {
      output("");
      output("Instruction Traps:");
      strcpy(line,"");
      for (i=0; i<256; i++)
        if (traps[i] != 0) {
          sprintf(tmp,"  %02X",i);
          strcat(line, tmp);
          if (strlen(line) > 58) {
            output(line);
            strcpy(line,"");
            }
          }
      if (strlen(line) > 0) output(line);
      }
    else {
      printf("Instruction Traps:\n");
      for (i=0; i<256; i++)
        if (traps[i] != 0) printf("  %02X\n",i);
      printf("\n");
      }
    }
  else if (*buffer == '+') {
    buffer++;
    hexToBin(buffer, &a);
    traps[a & 0xff] = 0xff;
    }
  else if (*buffer == '-') {
    buffer++;
    hexToBin(buffer, &a);
    traps[a & 0xff] = 0x00;
    }
  else if (*buffer == '=') {
    buffer++;
    hexToBin(buffer, &a);
    cpu->t=a & 0xff;
    }
  else {
    sprintf(line, "T=%02X",cpu->t);
    if (useVisual) output(line);
      else printf("%s\n",line);
    }
  }

void dbgCmdU(CPU* cpu, char* buffer) {
  if (*buffer == '+') liveUpdate = 0xff;
  if (*buffer == '-') liveUpdate = 0x00;
  }

void dbgCmdX(CPU* cpu, char* buffer) {
  word a;
  char line[80];
  if (*buffer == '=') {
    buffer++;
    hexToBin(buffer, &a);
    cpu->x=a & 0xf;
    }
  else {
    sprintf(line, "X=%X",cpu->x);
    if (useVisual) output(line);
      else printf("%s\n",line);
    }
  }

#define OP_MUL  0x60
#define OP_DIV  0x61
#define OP_MOD  0x62
#define OP_ADD  0x50
#define OP_SUB  0x51
#define OP_AND  0x40
#define OP_OR   0x41
#define OP_XOR  0x42
#define OP_EQ   0x30
#define OP_NE   0x31
#define OP_LT   0x32
#define OP_GT   0x33
#define OP_LTE  0x34
#define OP_GTE  0x35
#define OP_LAND 0x20
#define OP_LOR  0x21
#define OP_OP   0x10
#define OP_END  0x01
#define OP_NUM  0x00

int evaluateCondition(CPU* cpu, int num) {
  word numbers[256];
  char ops[256];
  char *pos;
  int  nstack;
  int  ostack;
  byte op;
  nstack = 0;
  ostack = 0;
  pos = conditions[num];
  while (*pos != 0) {
    if ((*pos == 'd' || *pos == 'D') &&
        (*(pos+1) != 'f' && *(pos+1) != 'F')) numbers[nstack++] = cpu->d;
    else if ((*pos == 'd' || *pos == 'D') &&
             (*(pos+1) == 'f' || *(pos+1) == 'F')) numbers[nstack++] = cpu->df;
    else if (*pos == 'x' || *pos == 'X') numbers[nstack++] = cpu->x;
    else if (*pos == 'p' || *pos == 'P') numbers[nstack++] = cpu->p;
    else if (*pos == 'q' || *pos == 'Q') numbers[nstack++] = cpu->q;
    else if (*pos == 't' || *pos == 'T') numbers[nstack++] = cpu->t;
    else if ((*pos == 'i' || *pos == 'I') &&
             (*(pos+1) == 'e' || *(pos+1) == 'E')) numbers[nstack++] = cpu->ie;
    else if (*pos == 'r' || *pos == 'R') {
      pos++;
      if (*pos >= '0' && *pos <= '9') numbers[nstack++] = cpu->r[*pos - '0'];
      if (*pos >= 'a' && *pos <= 'f') numbers[nstack++] = cpu->r[*pos - 87];
      if (*pos >= 'A' && *pos <= 'F') numbers[nstack++] = cpu->r[*pos - 55];
      }
    else if (*pos >= '0' && *pos <= '9') {
      numbers[nstack] = 0;
      while ((*pos >= '0' && *pos <= '9') ||
             (*pos >= 'a' && *pos <= 'f') ||
             (*pos >= 'A' && *pos <= 'F')) {
        numbers[nstack] <<= 4;
        if (*pos >= '0' && *pos <= '9') numbers[nstack] |= (*pos - '0');
        if (*pos >= 'a' && *pos <= 'f') numbers[nstack] |= (*pos - 87);
        if (*pos >= 'A' && *pos <= 'F') numbers[nstack] |= (*pos - 55);
        pos++;
        }
      nstack++;
      pos--;
      }
    else if (*pos == '[') {
      pos++;
      numbers[nstack] = 0;
      while ((*pos >= '0' && *pos <= '9') ||
             (*pos >= 'a' && *pos <= 'f') ||
             (*pos >= 'A' && *pos <= 'F')) {
        numbers[nstack] <<= 4;
        if (*pos >= '0' && *pos <= '9') numbers[nstack] |= (*pos - '0');
        if (*pos >= 'a' && *pos <= 'f') numbers[nstack] |= (*pos - 87);
        if (*pos >= 'A' && *pos <= 'F') numbers[nstack] |= (*pos - 55);
        pos++;
        }
      numbers[nstack] = cpu->ram[numbers[nstack]];
      nstack++;
      if (*pos != ']') return 0;
      }
    else return 0;
    if (*pos != 0) pos++;
    while (*pos == ' ') pos++;
    op = 0;
    switch (*pos) {
      case 0  : op = OP_END; break;
      case '*': op = OP_MUL; break;
      case '/': op = OP_DIV; break;
      case '%': op = OP_MOD; break;
      case '+': op = OP_ADD; break;
      case '-': op = OP_SUB; break;
      case '^': op = OP_XOR; break;
      case '&':
           if (*(pos+1) == '&') {
             op = OP_LAND;
             pos++;
             }
           else op = OP_AND;
           break;
      case '|':
           if (*(pos+1) == '|') {
             op = OP_LOR;
             pos++;
             }
           else op = OP_OR;
           break;
      case '=':
           if (*(pos+1) == '=') {
             op = OP_EQ;
             pos++;
             }
           else op = OP_EQ;
           break;
      case '<':
           if (*(pos+1) == '=') {
             op = OP_LTE;
             pos++;
             }
           else if (*(pos+1) == '>') {
             op = OP_NE;
             pos++;
             }
           else op = OP_LT;
           break;
      case '>':
           if (*(pos+1) == '=') {
             op = OP_GTE;
             pos++;
             }
           else op = OP_GT;
           break;
      }
    if (op == 0) return 0;
    while (ostack > 0 && (ops[ostack-1] & 0xf0) >= (op & 0xf0)) {
      nstack--;
      ostack--;
      switch (ops[ostack]) {
        case OP_MUL : numbers[nstack-1] *= numbers[nstack]; break;
        case OP_DIV : numbers[nstack-1] /= numbers[nstack]; break;
        case OP_MOD : numbers[nstack-1] %= numbers[nstack]; break;
        case OP_ADD : numbers[nstack-1] += numbers[nstack]; break;
        case OP_SUB : numbers[nstack-1] -= numbers[nstack]; break;
        case OP_AND : numbers[nstack-1] &= numbers[nstack]; break;
        case OP_OR  : numbers[nstack-1] |= numbers[nstack]; break;
        case OP_LAND: numbers[nstack-1] &= numbers[nstack]; break;
        case OP_LOR : numbers[nstack-1] |= numbers[nstack]; break;
        case OP_XOR : numbers[nstack-1] ^= numbers[nstack]; break;
        case OP_EQ  : numbers[nstack-1] = (numbers[nstack-1] == numbers[nstack]); break;
        case OP_NE  : numbers[nstack-1] = (numbers[nstack-1] != numbers[nstack]); break;
        case OP_LT  : numbers[nstack-1] = (numbers[nstack-1] <  numbers[nstack]); break;
        case OP_GT  : numbers[nstack-1] = (numbers[nstack-1] >  numbers[nstack]); break;
        case OP_LTE : numbers[nstack-1] = (numbers[nstack-1] <= numbers[nstack]); break;
        case OP_GTE : numbers[nstack-1] = (numbers[nstack-1] >= numbers[nstack]); break;
        }
      }
    if (op != OP_END) {
      ops[ostack++] = op;
      pos++;
      while (*pos == ' ') pos++;
      }
    }
  if (nstack == 0) return 0;
  if (nstack != 1) return 0;
  if (numbers[0] != 0) return -1;
  return 0;
  }

void dbgRun(CPU* cpu, char* buffer) {
  int i;
  word a;
  if ((*buffer >= '0' && *buffer <= '9') ||
      (*buffer >= 'a' && *buffer <= 'f') ||
      (*buffer >= 'A' && *buffer <= 'F')) {
    hexToBin(buffer, &a);
    cpu->r[cpu->p] = a;
    }
  cpu->idle = 0;
  runFlag = 0xff;
  while (runFlag) {
    if (liveUpdate && useVisual) {
      updateScreen(cpu);
      }
    cpuCycle(cpu);
    if (cpu->idle) runFlag = 0;
    for (i=0; i<numBreakpoints; i++)
      if (breakpoints[i] == cpu->r[cpu->p]) runFlag = 0;
    if (traps[cpu->ram[cpu->r[cpu->p]]] != 0) runFlag = 0;
    if (useConditions) {
      for (i=0; i<numConditions; i++)
        if (evaluateCondition(cpu, i)) runFlag = 0;
      }
    }
  }

void help() {
  char buffer[256];
  if (useVisual) {
    output("?              - dump memory");
    output("?P             - dump memory from R[P]");
    output("?X             - dump memory from R[X]");
    output("?Rn            - dump memory from R[n]");
    output("?addr          - dump memory from specified address");
    output("??             - show CPU registers");
    output("?I             - show input port mappings");
    output("?M             - show memory map");
    output("?O             - show output port mappings");
    output("$              - disassemble memory");
    output("$P             - disassemble memory from R[P]");
    output("$X             - disassemble memory from R[X]");
    output("$Rn            - disassemble memory from R[n]");
    output("$addr          - disassemble memory from specified address");
    output("!addr bb bb .. - store bytes at address");
    output("!P bb bb  ..   - store bytes at M[R[P]]");
    printf("\e[23;1H--MORE--");
    fgets(buffer,255,stdin);
    output("!X bb bb  ..   - store bytes at M[R[X]]");
    output("!Rn bb bb  ..  - store bytes at M[R[n]]");
    output("+ bb bb ..     - push values to stack (STXD)");
    output("-n             - pop n items from stack (IRX * n)");
    output("@              - run at R[P]");
    output("@addr          - set R[P] to addr then run");
    output("B?             - show break points");
    output("B+addr         - add break point");
    output("B-addr         - remove break point");
    output("C+expr         - add break point condition");
    output("C-num          - remove break point condition");
    output("C?             - show break point conditions");
    output("C++            - enable break point conditions");
    output("C--            - disable break point conditions");
    output("D              - show value in D");
    output("D=b            - Set D to b");
    printf("\e[23;1H--MORE--");
    fgets(buffer,255,stdin);
    output("DF             - show value in DF");
    output("DF=b           - set value in DF");
    output("DI bb          - perform DMA In using bb as input");
    output("DO             - perform DMA Out");
    output("I              - trigger interrupt action");
    output("IE             - show state of IE");
    output("IE=b           - set IE");
    output("P              - show value of P");
    output("P=n            - set P to n");
    output("Q              - show value of Q");
    output("Q=n            - set Q to n");
    output("R              - perform CPU reset");
    output("Rn             - show value of Rn");
    output("Rn=xxxx        - set Rn to xxxx");
    output("T              - show value of T");
    output("T=n            - set T to n");
    printf("\e[23;1H--MORE--");
    fgets(buffer,255,stdin);
    output("T?             - show instruction traps");
    output("T+bb           - add instruction trap");
    output("T-bb           - remove instruction trap");
    output("TR+            - turn on tracing");
    output("TR-            - turn off tracing");
    output("U+             - turn on live update");
    output("U-             - turn off live update");
    output("X              - show value of X");
    output("X=n            - set X to n");
    output("/              - exit");
    }
  else {
    printf("?              - dump memory\n");
    printf("?P             - dump memory from R[P]\n");
    printf("?X             - dump memory from R[X]\n");
    printf("?Rn            - dump memory from R[n]\n");
    printf("?addr          - dump memory from specified address\n");
    printf("??             - show CPU registers\n");
    printf("?I             - show input port mappings");
    printf("?M             - show memory map");
    printf("?O             - show output port mappings");
    printf("$              - disassemble memory\n");
    printf("$P             - disassemble memory from R[P]\n");
    printf("$X             - disassemble memory from R[X]\n");
    printf("$Rn            - disassemble memory from R[n]\n");
    printf("$addr          - disassemble memory from specified address\n");
    printf("!addr bb bb .. - store bytes at address\n");
    printf("!P bb bb  ..   - store bytes at M[R[P]]\n");
    printf("!X bb bb  ..   - store bytes at M[R[X]]\n");
    printf("!Rn bb bb  ..  - store bytes at M[R[n]]\n");
    printf("+ bb bb ..     - push values to stack (STXD)\n");
    printf("-n             - pop n items from stack (IRX * n)\n");
    printf("@              - run at R[P]\n");
    printf("@addr          - set R[P] to addr then run\n");
    printf("B?             - show break points\n");
    printf("B+addr         - add break point\n");
    printf("B-addr         - remove break point\n");
    printf("C+expr         - add break point condition\n");
    printf("C-num          - remove break point condition\n");
    printf("C?             - show break point conditions\n");
    printf("C++            - enable break point conditions\n");
    printf("C--            - disable break point conditions\n");
    printf("D              - show value in D\n");
    printf("D=b            - Set D to b\n");
    printf("DF             - show value in DF\n");
    printf("DF=b           - set value in DF\n");
    printf("DI bb          - perform DMA In using bb as input\n");
    printf("DO             - perform DMA Out\n");
    printf("I              - trigger interrupt action\n");
    printf("IE             - show state of IE\n");
    printf("IE=b           - set IE\n");
    printf("P              - show value of P\n");
    printf("P=n            - set P to n\n");
    printf("Q              - show value of Q\n");
    printf("Q=n            - set Q to n\n");
    printf("R              - perform CPU reset\n");
    printf("Rn             - show value of Rn\n");
    printf("Rn=xxxx        - set Rn to xxxx\n");
    printf("T              - show value of T\n");
    printf("T=n            - set T to n\n");
    printf("T?             - show instruction traps\n");
    printf("T+bb           - add instruction trap\n");
    printf("T-bb           - remove instruction trap\n");
    printf("TR+            - turn on tracing\n");
    printf("TR-            - turn off tracing\n");
    printf("U+             - turn on live update\n");
    printf("U-             - turn off live update\n");
    printf("X              - show value of X\n");
    printf("X=n            - set X to n\n");
    printf("/              - exit\n");
    }
  }

void debugger(CPU* cpu) {
  int i;
  byte flag;
  char buffer[1024];
  d_address = 0;
  flag = 0xff;
  numBreakpoints = 0;
  for (i=0; i<256; i++) traps[i] = 0;
  if (useVisual) printf("\e[H\e[2J");
  if (useVisual) showScreen(cpu);
  while (flag) {
    if (useVisual) updateScreen(cpu);
    printf("-->");
    disassem(cpu, cpu->r[cpu->p]);
    printf("%s\n",dline);
    printf("%04x>",cpu->r[cpu->p]);
    fgets(buffer,1023,stdin);
    while (strlen(buffer) > 0 && buffer[strlen(buffer)-1] < ' ')
      buffer[strlen(buffer)-1] = 0;
    if (buffer[0] == '?') dbgShow(cpu, buffer+1);
    if (buffer[0] == '$') dbgDisassem(cpu, buffer+1);
    if (buffer[0] == '!') dbgStore(cpu, buffer+1);
    if (buffer[0] == '@') dbgRun(cpu, buffer+1);
    if (buffer[0] == '+') dbgPush(cpu, buffer+1);
    if (buffer[0] == '-') dbgPop(cpu, buffer+1);
    if (buffer[0] == 'h' || buffer[0] == 'H') help();
    if (buffer[0] == 'b' || buffer[0] == 'B') dbgCmdB(cpu, buffer+1);
    if (buffer[0] == 'c' || buffer[0] == 'C') dbgCmdC(cpu, buffer+1);
    if (buffer[0] == 'd' || buffer[0] == 'D') dbgCmdD(cpu, buffer+1);
    if (buffer[0] == 'p' || buffer[0] == 'P') dbgCmdP(cpu, buffer+1);
    if (buffer[0] == 'r' || buffer[0] == 'R') dbgCmdR(cpu, buffer+1);
    if (buffer[0] == 'x' || buffer[0] == 'X') dbgCmdX(cpu, buffer+1);
    if (buffer[0] == 'i' || buffer[0] == 'I') dbgCmdI(cpu, buffer+1);
    if (buffer[0] == 'q' || buffer[0] == 'Q') dbgCmdQ(cpu, buffer+1);
    if (buffer[0] == 't' || buffer[0] == 'T') dbgCmdT(cpu, buffer+1);
    if (buffer[0] == 'u' || buffer[0] == 'U') dbgCmdU(cpu, buffer+1);
    if (buffer[0] == '/') flag = 0;
    if (buffer[0] == 0) {
      cpuCycle(cpu);
      }
    }
  }

