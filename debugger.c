#include "header.h"

word d_address;
word breakpoints[1024];
int  numBreakpoints;
byte traps[256];

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
                printf("68 %02x       STPC\n",inst);
                break;
           case 0x01:
                printf("68 %02x       DTC\n",inst);
                break;
           case 0x02:
                printf("68 %02x       SPM2\n",inst);
                break;
           case 0x03:
                printf("68 %02x       SCM2\n",inst);
                break;
           case 0x04:
                printf("68 %02x       SPM1\n",inst);
                break;
           case 0x05:
                printf("68 %02x       SCM1\n",inst);
                break;
           case 0x06:
                printf("68 %02x       LDC\n",inst);
                break;
           case 0x07:
                printf("68 %02x       STM\n",inst);
                break;
           case 0x08:
                printf("68 %02x       GEC\n",inst);
                break;
           case 0x09:
                printf("68 %02x       ETQ\n",inst);
                break;
           case 0x0a:
                printf("68 %02x       XIE\n",inst);
                break;
           case 0x0b:
                printf("68 %02x       XID\n",inst);
                break;
           case 0x0c:
                printf("68 %02x       CIE\n",inst);
                break;
           case 0x0d:
                printf("68 %02x       CID\n",inst);
                break;
           }
         break;
    case 0x20:
         adr = cpu->ram[address++] << 8;
         adr |= cpu->ram[address++];
         printf("68 %02x %02x %02x DBNZ   R%X,%04X\n",inst,(adr & 0xff00)>>8,adr&0xff,n,adr);
         break;
    case 0x30:
         switch (n) {
           case 0x0e:
                adr = cpu->ram[address++];
                printf("68 %02x %02x    BCI    %02X\n",inst,adr&0xff,adr&0xff);
                break;
           case 0x0f:
                adr = cpu->ram[address++];
                printf("68 %02x %02x    BXI    %02X\n",inst,adr&0xff,adr&0xff);
                break;
           }
    case 0x60:
         printf("68 %02x       RLXA   R%X\n",inst,n);
         break;
    case 0x70:
         switch (n) {
           case 0x04:
                printf("68 %02x       DADC\n",inst);
                break;
           case 0x06:
                printf("68 %02x       DSAV\n",inst);
                break;
           case 0x07:
                printf("68 %02x       DSMB\n",inst);
                break;
           case 0x0c:
                adr = cpu->ram[address++];
                printf("68 %02x %02x    DACI   %02X\n",inst,adr&0xff,adr&0xff);
                break;
           case 0x0f:
                adr = cpu->ram[address++];
                printf("68 %02x %02x    DSBI   %02X\n",inst,adr&0xff,adr&0xff);
                break;
           }
         break;
    case 0x80:
         adr = cpu->ram[address++] << 8;
         adr |= cpu->ram[address++];
         printf("68 %02x %02x %02x SCAL   R%X,%04X\n",inst,(adr & 0xff00)>>8,adr&0xff,n,adr);
         break;
    case 0x90:
         printf("68 %02x       SRET   R%X\n",inst,n);
         break;
    case 0xa0:
         printf("68 %02x       RSXD   R%X\n",inst,n);
         break;
    case 0xb0:
         printf("68 %02x       RNX    R%X\n",inst,n);
         break;
    case 0xc0:
         adr = cpu->ram[address++] << 8;
         adr |= cpu->ram[address++];
         printf("68 %02x %02x %02x RLDI   R%X,%04X\n",inst,(adr & 0xff00)>>8,adr&0xff,n,adr);
         break;
    case 0xf0:
         switch (n) {
           case 0x04:
                printf("68 %02x       DADD\n",inst);
                break;
           case 0x07:
                printf("68 %02x       DSM\n",inst);
                break;
           case 0x0c:
                adr = cpu->ram[address++];
                printf("68 %02x %02x    DADI   %02X\n",inst,adr&0xff,adr&0xff);
                break;
           case 0x0f:
                adr = cpu->ram[address++];
                printf("68 %02x %02x    DSMI   %02X\n",inst,adr&0xff,adr&0xff);
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
  printf("%04x: ",address);
  inst = cpu->ram[address++];
  i = inst & 0xf0;
  n = inst & 0x0f;
  switch (i) {
    case 0x00:
         if (n == 0)
           printf("00          IDL\n");
         else
           printf("%02x          LDN    %X\n",inst,n);
         break;
    case 0x10:
         printf("%02x          INC    %X\n",inst,n);
         break;
    case 0x20:
         printf("%02x          DEC    %X\n",inst,n);
         break;
    case 0x30:
         adr = cpu->ram[address++];
         switch (n) {
           case 0x00:
                printf("%02x %02x       BR     %02X\n",inst,adr,adr);
                break;
           case 0x01:
                printf("%02x %02x       BQ     %02X\n",inst,adr,adr);
                break;
           case 0x02:
                printf("%02x %02x       BZ     %02X\n",inst,adr,adr);
                break;
           case 0x03:
                printf("%02x %02x       BDF    %02X\n",inst,adr,adr);
                break;
           case 0x04:
                printf("%02x %02x       B1     %02X\n",inst,adr,adr);
                break;
           case 0x05:
                printf("%02x %02x       B2     %02X\n",inst,adr,adr);
                break;
           case 0x06:
                printf("%02x %02x       B3     %02X\n",inst,adr,adr);
                break;
           case 0x07:
                printf("%02x %02x       B4     %02X\n",inst,adr,adr);
                break;
           case 0x08:
                printf("%02x          SKP\n",inst);
                address--;
                break;
           case 0x09:
                printf("%02x %02x       BNQ    %02X\n",inst,adr,adr);
                break;
           case 0x0a:
                printf("%02x %02x       BNZ    %02X\n",inst,adr,adr);
                break;
           case 0x0b:
                printf("%02x %02x       BNF    %02X\n",inst,adr,adr);
                break;
           case 0x0c:
                printf("%02x %02x       BN1    %02X\n",inst,adr,adr);
                break;
           case 0x0d:
                printf("%02x %02x       BN2    %02X\n",inst,adr,adr);
                break;
           case 0x0e:
                printf("%02x %02x       BN3    %02X\n",inst,adr,adr);
                break;
           case 0x0f:
                printf("%02x %02x       BN4    %02X\n",inst,adr,adr);
                break;
           }
         break;
    case 0x40:
         printf("%02x          LDA    %X\n",inst,n);
         break;
    case 0x50:
         printf("%02x          STR    %X\n",inst,n);
         break;
    case 0x60:
         switch (n) {
           case 0x00:
                printf("%02x          IRX\n",inst);
                break;
           case 0x01:
                printf("%02x          OUT    1\n",inst);
                break;
           case 0x02:
                printf("%02x          OUT    2\n",inst);
                break;
           case 0x03:
                printf("%02x          OUT    3\n",inst);
                break;
           case 0x04:
                printf("%02x          OUT    4\n",inst);
                break;
           case 0x05:
                printf("%02x          OUT    5\n",inst);
                break;
           case 0x06:
                printf("%02x          OUT    6\n",inst);
                break;
           case 0x07:
                printf("%02x          OUT    7\n",inst);
                break;
           case 0x08:
                address = disassem1805(cpu, address);
                break;
           case 0x09:
                printf("%02x          INP    1\n",inst);
                break;
           case 0x0a:
                printf("%02x          INP    2\n",inst);
                break;
           case 0x0b:
                printf("%02x          INP    3\n",inst);
                break;
           case 0x0c:
                printf("%02x          INP    4\n",inst);
                break;
           case 0x0d:
                printf("%02x          INP    5\n",inst);
                break;
           case 0x0e:
                printf("%02x          INP    6\n",inst);
                break;
           case 0x0f:
                printf("%02x          INP    7\n",inst);
                break;
           }
         break;
    case 0x70:
         switch (n) {
           case 0x00:
                printf("%02x          RET\n",inst);
                break;
           case 0x01:
                printf("%02x          DIS\n",inst);
                break;
           case 0x02:
                printf("%02x          LDXA\n",inst);
                break;
           case 0x03:
                printf("%02x          STXD\n",inst);
                break;
           case 0x04:
                printf("%02x          ADC\n",inst);
                break;
           case 0x05:
                printf("%02x          SDB\n",inst);
                break;
           case 0x06:
                printf("%02x          SHRC\n",inst);
                break;
           case 0x07:
                printf("%02x          SMB\n",inst);
                break;
           case 0x08:
                printf("%02x          SAV\n",inst);
                break;
           case 0x09:
                printf("%02x          MARK\n",inst);
                break;
           case 0x0a:
                printf("%02x          REQ\n",inst);
                break;
           case 0x0b:
                printf("%02x          SEQ\n",inst);
                break;
           case 0x0c:
                adr = cpu->ram[address++];
                printf("%02x %02x         ADCI   %02X\n",inst,adr,adr);
                break;
           case 0x0d:
                adr = cpu->ram[address++];
                printf("%02x %02x         SDBI   %02X\n",inst,adr,adr);
                break;
           case 0x0e:
                printf("%02x          SHLC\n",inst);
                break;
           case 0x0f:
                adr = cpu->ram[address++];
                printf("%02x %02x         SMBI   %02X\n",inst,adr,adr);
                break;
           }
         break;
    case 0x80:
         printf("%02x          GLO    %X\n",inst,n);
         break;
    case 0x90:
         printf("%02x          GHI    %X\n",inst,n);
         break;
    case 0xa0:
         printf("%02x          PLO    %X\n",inst,n);
         break;
    case 0xb0:
         printf("%02x          PHI    %X\n",inst,n);
         break;
    case 0xc0:
         adr = cpu->ram[address++] << 8;
         adr |= cpu->ram[address++];
         switch (n) {
           case 0x00:
                printf("%02x %02x %02x    LBR    %04X\n",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x01:
                printf("%02x %02x %02x    LBQ    %04X\n",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x02:
                printf("%02x %02x %02x    LBZ    %04X\n",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x03:
                printf("%02x %02x %02x    LBDF   %04X\n",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x04:
                printf("%02x            NOP\n",inst);
                address -= 2;
                break;
           case 0x05:
                printf("%02x            LSNQ\n",inst);
                address -= 2;
                break;
           case 0x06:
                printf("%02x            LSNZ\n",inst);
                address -= 2;
                break;
           case 0x07:
                printf("%02x            LSNF\n",inst);
                address -= 2;
                break;
           case 0x08:
                printf("%02x            LSKP\n",inst);
                address -= 2;
                break;
           case 0x09:
                printf("%02x %02x %02x    LBNQ   %04X\n",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x0a:
                printf("%02x %02x %02x    LBNZ   %04X\n",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x0b:
                printf("%02x %02x %02x    LBNF   %04X\n",inst,(adr & 0xff00)>>8,adr&0xff,adr);
                break;
           case 0x0c:
                printf("%02x            LSIE\n",inst);
                address -= 2;
                break;
           case 0x0d:
                printf("%02x            LSQ\n",inst);
                address -= 2;
                break;
           case 0x0e:
                printf("%02x            LSZ\n",inst);
                address -= 2;
                break;
           case 0x0f:
                printf("%02x            LSDF\n",inst);
                address -= 2;
                break;
           }
         break;
    case 0xd0:
         printf("%02x          SEP    %X\n",inst,n);
         break;
    case 0xe0:
         printf("%02x          SEX    %X\n",inst,n);
         break;
    case 0xf0:
         switch (n) {
           case 0x00:
                printf("%02x          LDX\n",inst);
                break;
           case 0x01:
                printf("%02x          OR\n",inst);
                break;
           case 0x02:
                printf("%02x          AND\n",inst);
                break;
           case 0x03:
                printf("%02x          XOR\n",inst);
                break;
           case 0x04:
                printf("%02x          ADD\n",inst);
                break;
           case 0x05:
                printf("%02x          SD\n",inst);
                break;
           case 0x06:
                printf("%02x          SHR\n",inst);
                break;
           case 0x07:
                printf("%02x          SM\n",inst);
                break;
           case 0x08:
                adr = cpu->ram[address++];
                printf("%02x %02x       LDI    %02X\n",inst,adr,adr);
                break;
           case 0x09:
                adr = cpu->ram[address++];
                printf("%02x %02x       ORI    %02X\n",inst,adr,adr);
                break;
           case 0x0a:
                adr = cpu->ram[address++];
                printf("%02x %02x       ANI    %02X\n",inst,adr,adr);
                break;
           case 0x0b:
                adr = cpu->ram[address++];
                printf("%02x %02x       XRI    %02X\n",inst,adr,adr);
                break;
           case 0x0c:
                adr = cpu->ram[address++];
                printf("%02x %02x       ADI    %02X\n",inst,adr,adr);
                break;
           case 0x0d:
                adr = cpu->ram[address++];
                printf("%02x %02x       SDI    %02X\n",inst,adr,adr);
                break;
           case 0x0e:
                printf("%02x          SHL\n",inst);
                break;
           case 0x0f:
                adr = cpu->ram[address++];
                printf("%02x %02x       SMI    %02X\n",inst,adr,adr);
                break;
           }
         break;

    }
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
  int x,y;
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

void dbgShow(CPU* cpu, char* buffer) {
  int x,y;
  word a;
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
    printf("%04x:", d_address);
    for (x=0; x<16; x++) {
      printf(" %02x",cpu->ram[d_address+x]);
      }

    printf("\n");
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

  for (i=0; i<20; i++)
    d_address = disassem(cpu, d_address);
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
  if (*buffer == '?') {
    printf("Break points:\n");
    for (i=0; i<numBreakpoints; i++)
      printf("  %04x\n",breakpoints[i]);
    printf("\n");
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

void dbgCmdD(CPU* cpu, char* buffer) {
  word a;
  if (*buffer == 'f' || *buffer == 'F') {
    buffer++;
    if (*buffer == '=') {
      buffer++;
      hexToBin(buffer, &a);
      cpu->df = a & 1;
      }
    else printf("DF=%d\n",cpu->df);
    }
  else if (*buffer == 'i' || *buffer == 'I') {
    buffer++;
    while (*buffer == ' ') buffer++;
    hexToBin(buffer, &a);
    cpuDmaIn(cpu, a & 0xff);
    }
  else if (*buffer == 'o' || *buffer == 'O') {
    printf("DMA OUT: %02x\n",cpuDmaOut(cpu));
    }
  else if (*buffer == '=') {
    buffer++;
    hexToBin(buffer, &a);
    cpu->d=a & 0xff;
    }
  else printf("D=%02X\n",cpu->d);
  }

void dbgCmdI(CPU* cpu, char* buffer) {
  word a;
  if (*buffer == 'e' || *buffer == 'E') {
    buffer++;
    if (*buffer == '=') {
      buffer++;
      hexToBin(buffer, &a);
      cpu->ie = a & 1;
      }
    else printf("IE=%X\n",cpu->ie);
    }
  else cpuIntr(cpu,'E');
  }

void dbgCmdP(CPU* cpu, char* buffer) {
  word a;
  if (*buffer == '=') {
    buffer++;
    hexToBin(buffer, &a);
    cpu->p=a & 0xf;
    }
  else printf("P=%X\n",cpu->p);
  }

void dbgCmdQ(CPU* cpu, char* buffer) {
  word a;
  if (*buffer == '=') {
    buffer++;
    hexToBin(buffer, &a);
    cpu->q=a & 0x1;
    }
  else printf("Q=%X\n",cpu->q);
  }

void dbgCmdR(CPU* cpu, char* buffer) {
  word a;
  byte r;
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
  else printf("R%X=%X\n",r,cpu->r[r]);
  }

void dbgCmdT(CPU* cpu, char* buffer) {
  int i;
  word a;
  if (*buffer == '?') {
    printf("Instructions Traps:\n");
    for (i=0; i<256; i++)
      if (traps[i] != 0) printf("  %02X\n",i);
    printf("\n");
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
  else printf("T=%02X\n",cpu->t);
  }

void dbgCmdX(CPU* cpu, char* buffer) {
  word a;
  if (*buffer == '=') {
    buffer++;
    hexToBin(buffer, &a);
    cpu->x=a & 0xf;
    }
  else printf("X=%X\n",cpu->x);
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
    cpuCycle(cpu);
    if (cpu->idle) runFlag = 0;
    for (i=0; i<numBreakpoints; i++)
      if (breakpoints[i] == cpu->r[cpu->p]) runFlag = 0;
    if (traps[cpu->ram[cpu->r[cpu->p]]] != 0) runFlag = 0;
    }
  }

void help() {
  printf("?              - dump memory\n");
  printf("?P             - dump memory from R[P]\n");
  printf("?X             - dump memory from R[X]\n");
  printf("?Rn            - dump memory from R[n]\n");
  printf("?addr          - dump memory from specified address\n");
  printf("??             - show CPU registers\n");
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
  printf("X              - show value of X\n");
  printf("X=n            - set X to n\n");
  printf("/              - exit\n");
  }

void debugger(CPU* cpu) {
  int i;
  byte flag;
  byte buffer[1024];
  d_address = 0;
  flag = 0xff;
  numBreakpoints = 0;
  for (i=0; i<256; i++) traps[i] = 0;
  while (flag) {
    printf("-->");
    disassem(cpu, cpu->r[cpu->p]);
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
    if (buffer[0] == 'd' || buffer[0] == 'D') dbgCmdD(cpu, buffer+1);
    if (buffer[0] == 'p' || buffer[0] == 'P') dbgCmdP(cpu, buffer+1);
    if (buffer[0] == 'r' || buffer[0] == 'R') dbgCmdR(cpu, buffer+1);
    if (buffer[0] == 'x' || buffer[0] == 'X') dbgCmdX(cpu, buffer+1);
    if (buffer[0] == 'i' || buffer[0] == 'I') dbgCmdI(cpu, buffer+1);
    if (buffer[0] == 'q' || buffer[0] == 'Q') dbgCmdQ(cpu, buffer+1);
    if (buffer[0] == 't' || buffer[0] == 'T') dbgCmdT(cpu, buffer+1);
    if (buffer[0] == '/') flag = 0;
    if (buffer[0] == 0) {
      cpuCycle(cpu);
      }
    }
  }

