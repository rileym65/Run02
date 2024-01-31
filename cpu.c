#include "header.h"

char tbuffer[256];
char tline[80];

#define T_COUNT1      1
#define T_COUNT2      2
#define T_PULSE1      3
#define T_PULSE2      4
#define T_TIMER       5

byte readMem(CPU* cpu, word addr) {
  if (addr == 0xfff9) return 0x01;
  if (addr == 0xfffa) return 0x00;
  if (addr == 0xfffb) return 0x0c;
  if (mmap[(addr & 0xff00) >> 8] != 'X') return cpu->ram[addr];
  return 0xff;
  }

void writeMem(CPU* cpu, word addr, byte value) {
  if (mmap[(addr & 0xff00) >> 8] == 'A') cpu->ram[addr] = value;

  }

void cpuReset(CPU *cpu) {
  cpu->p = 0;
  cpu->x = 0;
  cpu->r[0] = 0;
  cpu->ie = 1;
  cpu->idle = 0;
  cpu->ef[0] = 0;
  cpu->ef[1] = 0;
  cpu->ef[2] = 0;
  cpu->ef[3] = 0;
  cpu->ci = 0;
  cpu->xi = 0;
  cpu->cie = 1;
  cpu->xie = 1;
  cpu->crunning = 0;
  cpu->etq = 0;
  cpu->cpre = 0;
  }

void cpuIntr(CPU* cpu, char typ) {
  if (cpu->ie == 0) return;
  if (typ == 'C' && cpu->cie == 0) return;
  if (typ == 'E' && cpu->xie == 0) return;
  cpu->t = (cpu->x << 4) | cpu->p;
  cpu->x = 2;
  cpu->p = 1;
  cpu->ie = 0;
  }

void cpuDmaIn(CPU* cpu, byte v) {
  cpu->ram[cpu->r[0]++] = v;
  }

byte cpuDmaOut(CPU* cpu) {
  return cpu->ram[cpu->r[0]++];
  }

void sret(CPU *cpu) {
  cpu->r[3] = cpu->r[6];
  cpu->x = 2;
  cpu->r[cpu->x]++;
  cpu->r[6] = (cpu->ram[cpu->r[cpu->x]++]);
  cpu->r[6] |= ((cpu->ram[cpu->r[cpu->x]]) << 8);
  cpu->p = 3;
//  cpu->r[5] = 0xfa8d;
  }

void bcdAdd(CPU* cpu,byte a, byte b, byte c) {
  cpu->d = a+b+c;
  cpu->df = 0;
  if ((cpu->d & 0x0f) >= 0x0a || ((cpu->d & 0x0f) < (a & 0x0f))) cpu->d += 0x06;
  if ((cpu->d & 0xf0) >= 0xa0 || ((cpu->d & 0xf0) < (a & 0xf0))) { cpu->d += 0x60; cpu->df = 1; }
  if ((cpu->d & 0xf0) < (a & 0xf0)) cpu->df = 1;
  cycles++;
  }


void bcdSub(CPU* cpu,byte a, byte b, byte c) {
  b = 0x99 - b;
  if ((b & 0x0f) != 9) b++;
  else b = (b & 0xf0) + 0x10;
  c = (c == 0) ? 1 : 0;
  bcdAdd(cpu, a, b, c);
  }

void cpu1805(CPU *cpu) {
  byte i;
  word d;
  i = cpu->ram[cpu->r[cpu->p]++];
  cpu->n = i & 0xf;
  cpu->i = (i >> 4) & 0xff;
  cycles++;
  switch (cpu->i) {
    case 0x00:
         switch (cpu->n) {
           case 0x00:                                                      // STPC
                cpu->crunning = 0;
                cpu->cpre = 0;
                if (showTrace) {
                  sprintf(tline,"STPC");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x01:                                                      // DTC
                cpu->cntr--;
                if (showTrace) {
                  sprintf(tline,"DTC");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x02:                                                      // SPM2
                cpu->cmode = T_PULSE2;
                cpu->cpre = 0;
                cpu->crunning = 0xff;
                if (showTrace) {
                  sprintf(tline,"SPM2");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x03:                                                      // SCM2
                cpu->cmode = T_COUNT2;
                cpu->cpre = 0;
                cpu->crunning = 0xff;
                if (showTrace) {
                  sprintf(tline,"SCM2");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x04:                                                      // SPM1
                cpu->cmode = T_PULSE1;
                cpu->cpre = 0;
                cpu->crunning = 0xff;
                if (showTrace) {
                  sprintf(tline,"SPM1");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x05:                                                      // SCM1
                cpu->cmode = T_COUNT1;
                cpu->cpre = 0;
                cpu->crunning = 0xff;
                if (showTrace) {
                  sprintf(tline,"SCM1");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x06:                                                      // LDC
                if (cpu->crunning) {
                  cpu->ch = cpu->d;
                  }
                else {
                  cpu->cntr = cpu->d;
                  cpu->ch = cpu->d;
                  cpu->ci = 0;
                  }
                if (showTrace) {
                  sprintf(tline,"LDC");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x07:                                                      // STM
                cpu->cmode = T_TIMER;
                cpu->cpre = 0;
                cpu->crunning = 0xff;
                if (showTrace) {
                  sprintf(tline,"STM");
                  }
                break;
           case 0x08:                                                      // GEC
                cpu->d = cpu->cntr;
                if (showTrace) {
                  sprintf(tline,"GEC                    D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x09:                                                      // ETQ
                cpu->etq = 1;
                break;
                if (showTrace) {
                  sprintf(tline,"ETQ");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0a:                                                      // XIE
                cpu->xie = 1;
                break;
                if (showTrace) {
                  sprintf(tline,"XIE");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0b:                                                      // XID
                cpu->xie = 0;
                break;
                if (showTrace) {
                  sprintf(tline,"XID");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0c:                                                      // CIE
                cpu->cie = 1;
                break;
                if (showTrace) {
                  sprintf(tline,"CIE");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0d:                                                      // CID
                cpu->cie = 0;
                break;
                if (showTrace) {
                  sprintf(tline,"CID");
                  strcat(tbuffer, tline);
                  }
                break;
           }
         break;
    case 0x02:                                                             // DBNZ
         cpu->r[cpu->n]--;
         d = cpu->ram[cpu->r[cpu->p]++] << 8;
         d |= cpu->ram[cpu->r[cpu->p]++];
         cycles += 2;
         if (showTrace) {
           sprintf(tline,"DBNZ  R%X,%04X        R%X=%04x ",cpu->n,d,cpu->n,cpu->r[cpu->n]);
           strcat(tbuffer, tline);
           }
         if (cpu->r[cpu->n] != 0) {
           cpu->r[cpu->p] = d;
           if (showTrace) strcat(tline,"*");
           }
         else {
           if (showTrace) strcat(tline,"");
           }
         break;
    case 0x03:
         switch (cpu->n) {
           case 0x0e:                                                      // BCI
                d = cpu->ram[(cpu->r[cpu->p]+1) & 0xffff];
                if (showTrace) {
                  sprintf(tline,"BCI   %02X             ",d);
                  strcat(tbuffer, tline);
                  }
                if (cpu->ci) {
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) | (d & 0x00ff);
                  cpu->ci = 0;
                  if (showTrace) strcat(tline,"*");
                  }
                else {
                  cpu->r[cpu->p]++;
                  if (showTrace) strcat(tline,"");
                  }
                break;
           case 0x0f:                                                      // BXI
                d = cpu->ram[(cpu->r[cpu->p]+1) & 0xffff];
                if (showTrace) {
                  sprintf(tline,"BXI   %02X             ",d);
                  strcat(tbuffer, tline);
                  }
                if (cpu->xi) {
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) | (d & 0x00ff);
                  cpu->xi = 0;
                  if (showTrace) strcat(tline,"*");
                  }
                else {
                  cpu->r[cpu->p]++;
                  if (showTrace) strcat(tline,"");
                  }
                break;
           }
           break;
    case 0x06:                                                             // RLXA
         cpu->r[cpu->n] = cpu->ram[cpu->r[cpu->x]++] << 8;
         cpu->r[cpu->n] |= cpu->ram[cpu->r[cpu->x]++];
         cycles += 2;
         if (showTrace) {
           sprintf(tline,"RLXA  R%X             R%X=%04x",cpu->n,cpu->n,cpu->r[cpu->n]);
           strcat(tbuffer, tline);
           }
         break;
    case 0x07:
         switch (cpu->n) {
           case 0x04:                                                      // DADC
                bcdAdd(cpu, cpu->d, cpu->ram[cpu->r[cpu->x]], cpu->df);
                if (showTrace) {
                  sprintf(tline,"DADC                   D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x06:                                                      // DSAV
                cpu->r[cpu->x]--;
                cpu->ram[cpu->r[cpu->x]] = cpu->t;
                cpu->r[cpu->x]--;
                cpu->ram[cpu->r[cpu->x]] = cpu->d;
                cpu->r[cpu->x]--;
                cpu->d <<= 1;
                if (cpu->df != 0) cpu->d |= 1;
                cpu->ram[cpu->r[cpu->x]] = cpu->d;
                cycles += 3;
                if (showTrace) {
                  sprintf(tline,"DSAV");
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x07:                                                      // DSMB
                bcdSub(cpu, cpu->d, cpu->ram[cpu->r[cpu->x]], cpu->df);
                if (showTrace) {
                  sprintf(tline,"DSMB                   D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0c:                                                      // DACI
                bcdAdd(cpu, cpu->d, cpu->ram[cpu->r[cpu->p]++], cpu->df);
                if (showTrace) {
                  sprintf(tline,"DACI                   D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0f:                                                      // DSBI
                bcdSub(cpu, cpu->d, cpu->ram[cpu->r[cpu->p]++], cpu->df);
                if (showTrace) {
                  sprintf(tline,"DSBI                   D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           }
         break;
    case 0x08:                                                             // SCAL
         cpu->ram[cpu->r[cpu->x]--] = (cpu->r[cpu->n] & 0x00ff);
         cpu->ram[cpu->r[cpu->x]--] = ((cpu->r[cpu->n] & 0xff00) >> 8);
         cpu->r[cpu->n] = cpu->r[cpu->p];
         d = (cpu->ram[cpu->r[cpu->n]++] << 8);
         d |= cpu->ram[cpu->r[cpu->n]++];
         cpu->r[cpu->p] = d;
         cycles += 7;
         if (showTrace) {
           sprintf(tline,"SCAL  R%X,%04X",cpu->n,d);
           strcat(tbuffer, tline);
           }
         break;
    case 0x09:                                                             // SRET
         cpu->r[cpu->p] = cpu->r[cpu->n];
         cpu->r[cpu->x]++;
         cpu->r[cpu->n] = (cpu->ram[cpu->r[cpu->x]] << 8);
         cpu->r[cpu->x]++;
         cpu->r[cpu->n] |= cpu->ram[cpu->r[cpu->x]];
         cycles += 5;
         if (showTrace) {
           sprintf(tline,"SRET  R%X",cpu->n);
           strcat(tbuffer, tline);
           }
         break;
    case 0x0a:                                                             // RSXD
         cpu->ram[cpu->r[cpu->x]--] = (cpu->r[cpu->n] >> 8);
         cpu->ram[cpu->r[cpu->x]--] = (cpu->r[cpu->n] & 0xff);
         cycles += 2;
         if (showTrace) {
           sprintf(tline,"RSXD  R%X             M[X]=%04x",cpu->n,cpu->r[cpu->n]);
           strcat(tbuffer, tline);
           }
         break;
    case 0x0b:                                                             // RNX
         cpu->r[cpu->x] = cpu->r[cpu->n];
         cycles++;
         if (showTrace) {
           sprintf(tline,"RNX   R%X             R%X=%04x",cpu->x,cpu->x,cpu->r[cpu->x]);
           strcat(tbuffer, tline);
           }
         break;
    case 0x0c:                                                             // RLDI
         cpu->r[cpu->n] = cpu->ram[cpu->r[cpu->p]++] << 8;
         cpu->r[cpu->n] |= cpu->ram[cpu->r[cpu->p]++];
         cycles += 2;
         if (showTrace) {
           sprintf(tline,"RLDI  R%X             R%X=%04x",cpu->n,cpu->n,cpu->r[cpu->n]);
           strcat(tbuffer, tline);
           }
         break;
    case 0x0f:
         switch (cpu->n) {
           case 0x04:                                                      // DADD
                bcdAdd(cpu, cpu->d, cpu->ram[cpu->r[cpu->x]], 0);
                if (showTrace) {
                  sprintf(tline,"DADD                   D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x07:                                                      // DSM
                bcdSub(cpu, cpu->d, cpu->ram[cpu->r[cpu->x]], 1);
                if (showTrace) {
                  sprintf(tline,"DSM                    D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0c:                                                      // DADI
                bcdAdd(cpu, cpu->d, cpu->ram[cpu->r[cpu->p]++], 0);
                if (showTrace) {
                  sprintf(tline,"DADI                   D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0f:                                                      // DSMI
                bcdSub(cpu, cpu->d, cpu->ram[cpu->r[cpu->p]++], 1);
                if (showTrace) {
                  sprintf(tline,"DSMI                   D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           }
         break;
    }
  }

void doAlloc(CPU* cpu) {
  word addr;
  word heap;
  word size;
  byte flags;
  byte flag;
  word mask;
  heap = (cpu->ram[0x468] << 8) | cpu->ram[0x469];
  if ((cpu->r[7] & 0xff00) != 0) {
    mask = (cpu->r[7] & 0xff00) >> 8;
    mask = mask ^ 0xffff;
    addr = heap - cpu->r[0xc];
    addr &= mask;
    size = heap - addr;
    heap = addr - 3;
    cpu->ram[heap] = 2 | (cpu->r[7] & 0x00ff);
    cpu->ram[heap+1] = (size & 0xff00) >> 8;
    cpu->ram[heap+2] = (size & 0x00ff);
    cpu->r[0xf] = addr;
    cpu->df = 0;
    cpu->r[0xc] = size;
    cpu->ram[0x468] = (heap & 0xff00) >> 8;
    cpu->ram[0x469] = (heap & 0x00ff);
    heap--;
    cpu->ram[0x442] = (heap & 0xff00) >> 8;
    cpu->ram[0x443] = (heap & 0x00ff);
    return;
    }
  else {
    flag = 0;
    while (flag == 0) {
      flags = cpu->ram[heap++];
      size  = cpu->ram[heap++] << 8;
      size |= cpu->ram[heap++];
      if (flags == 0) flag = 0xff;
      else if (flags == 1 && cpu->r[0xc] <= size) {
        addr = heap;
        cpu->ram[heap-3] = 2 | (cpu->r[7] & 0xff);
        cpu->r[0xf] = addr;
        if (size - cpu->r[0xc] >= 11) {
          cpu->ram[heap-2] = (cpu->r[0xc] & 0xff00) >> 8;
          cpu->ram[heap-1] = (cpu->r[0xc] & 0x00ff);
          heap = addr + cpu->r[0xc];
          size = size - (cpu->r[0xc] + 3);
          cpu->ram[heap++] = 0x01;
          cpu->ram[heap++] = (size & 0xff00) >> 8;
          cpu->ram[heap] = (size & 0x00ff);
          cpu->df = 0;
          return;
          }
        cpu->r[0xc] = size;
        cpu->df = 0;
        return;
        }
      else {
        heap += size;
        }
      }
    heap = (cpu->ram[0x468] << 8) | cpu->ram[0x469];
    heap -= (cpu->r[0xc] + 3);
    cpu->ram[heap] = 2 | (cpu->r[7] & 0xff);
    cpu->ram[heap+1] = (cpu->r[0xc] & 0xff00) >> 8;
    cpu->ram[heap+2] = (cpu->r[0xc] & 0x00ff);
    cpu->r[0xf] = heap+3;
    cpu->df = 0;
    cpu->ram[0x468] = (heap & 0xff00) >> 8;
    cpu->ram[0x469] = (heap & 0x00ff);
    heap--;
    cpu->ram[0x442] = (heap & 0xff00) >> 8;
    cpu->ram[0x443] = (heap & 0x00ff);
    cpu->df = 0;
    }
  }

void doDealloc(CPU* cpu) {
  word addr;
  word heap;
  word size;
  word size2;
  heap = (cpu->ram[0x468] << 8) | cpu->ram[0x469];
  cpu->ram[cpu->r[0xf]-3] = 1;
  while (cpu->ram[heap] != 0) {
    size = (cpu->ram[heap+1] << 8) | cpu->ram[heap+2];
    if (cpu->ram[heap] == 1) {
      addr = heap + size + 3;
      if (cpu->ram[addr] == 1) {
        size2 = (cpu->ram[addr+1] << 8) | cpu->ram[addr+2];
        size += size2 + 3;
        cpu->ram[heap+1] = (size & 0xff00) >> 8;
        cpu->ram[heap+2] = (size & 0x00ff);
        }
      else heap += size + 3;
      }
    else heap += size + 3;
    }
  heap = (cpu->ram[0x468] << 8) | cpu->ram[0x469];
  if (cpu->ram[heap] != 1) return;
  while (cpu->ram[heap] == 1) {
    size = (cpu->ram[heap+1] << 8) | cpu->ram[heap+2];
    heap += size + 3;
    }
  cpu->ram[0x468] = (heap & 0xff00) >> 8;
  cpu->ram[0x469] = (heap & 0x00ff);
  heap--;
  cpu->ram[0x442] = (heap & 0xff00) >> 8;
  cpu->ram[0x443] = (heap & 0x00ff);
  }

void ideBoot(CPU* cpu) {
  int disk;
  byte* ram;
  disk = open("disk1.ide", O_RDONLY);
  if (disk < 0) {
    printf("Attempt to boot non-existant disk, aborting.\n");
    exit(1);
    }
  cpu->r[4] = 0xfa7b;
  cpu->r[5] = 0xfa8d;
  cpu->r[3] = 0x0106;
  cpu->r[2] = 0x00f0;
  cpu->r[8] = 0xe000;
  cpu->r[7] = 0x0000;
  cpu->p = 3;
  cpu->x = 2;
  ram = cpu->ram;
  ram += 0x0100;
  read(disk, ram, 512);
  close(disk);
  }

void ideRead(CPU* cpu) {
  int disk;
  int drive;
  qword pos;
  byte* ram;
  disk = -1;
  drive = (cpu->r[8] & 0xf000) | 0xe000;
  if (drive == 0xe000) disk = open("disk1.ide", O_RDONLY);
  if (drive == 0xf000) disk = open("disk2.ide", O_RDONLY);
  if (disk < 0) {
    printf("Attempt to read non-existant disk, aborting.\n");
    exit(1);
    }
  pos = cpu->r[8] & 0x00000fff;
  pos = (pos << 16) | cpu->r[7];
  pos <<= 9;
  lseek(disk, pos, SEEK_SET);
  ram = cpu->ram;
  ram += cpu->r[15];
  read(disk, ram, 512);
  cpu->r[15] += 512;
  cpu->df = 0;
  close(disk);
  }

void ideWrite(CPU* cpu) {
  int disk;
  qword pos;
  byte* ram;
  int   drive;
  disk = -1;
  drive = (cpu->r[8] & 0xf000) | 0xe000;
  if (drive == 0xe000) disk = open("disk1.ide", O_RDWR);
  if (drive == 0xf000) disk = open("disk2.ide", O_RDWR);
  if (disk < 0) {
    printf("Attempt to write non-existant disk, aborting.\n");
    exit(1);
    }
  pos = cpu->r[8] & 0x00000fff;
  pos = (pos << 16) | cpu->r[7];
  pos <<= 9;
  lseek(disk, pos, SEEK_SET);
  ram = cpu->ram;
  ram += cpu->r[15];
  write(disk, ram, 512);
  cpu->r[15] += 512;
  cpu->df = 0;
  close(disk);
  }

void checkEOF(CPU *cpu) {
  int p;
  int s;
  int c;
  int f;
  p = (cpu->ram[cpu->r[0xd]+0] << 24) |
      (cpu->ram[cpu->r[0xd]+1] << 16) |
      (cpu->ram[cpu->r[0xd]+2] << 8 ) |
       cpu->ram[cpu->r[0xd]+3];
  f = cpu->ram[cpu->r[0xd]+9] << 24;
  f |= cpu->ram[cpu->r[0xd]+10] << 16;
  f |= cpu->ram[cpu->r[0xd]+11] << 8;
  f |= cpu->ram[cpu->r[0xd]+12];
  c = lseek(f, 0, SEEK_CUR);
  s = lseek(f, 0, SEEK_END);
      lseek(f, c, SEEK_SET);
  p &= 0xfffff000;
  s &= 0xfffff000;
  if (p == s)
    cpu->ram[cpu->r[0xd] + 8] |= 4;
  else
    cpu->ram[cpu->r[0xd] + 8] &= 0xfb;
  }

void cpuCycle(CPU *cpu) {
  byte i;
  byte key;
  int  f;
  int  flags;
  int  p;
  int  cycles;
  char buffer[32];
  char buffer2[2];
  char name2[32];
  struct timeval tv;
  time_t tim;
  struct tm dt;
  long long st;
  long long et;
  word w;
  if (showTrace) {
    sprintf(tbuffer,"R%x:[%04x] ",cpu->p,cpu->r[cpu->p]);
    buffer2[1] = 0;
    }
  if (useElfos) {
    switch (cpu->r[cpu->p]) {
      case 0x0306:                                                         // o_open
           i = 0;
           while (cpu->ram[cpu->r[0xf]] != 0)
             buffer[i++] = cpu->ram[cpu->r[0xf]++];
           buffer[i] = 0;
           flags = 0;
           if (cpu->r[0x7] & 1) flags |= O_CREAT;
           if (cpu->r[0x7] & 2) flags |= O_TRUNC;
           if (cpu->r[0x7] & 16) flags |= O_RDONLY;
             else flags |= O_RDWR;
           f = open(buffer, flags, 0666);
           if (f < 0) {
             cpu->df = 1;
             cpu->d = 4;
             sret(cpu);
             return;
             }
           cpu->ram[cpu->r[0xd]+8] = 8;
           p = lseek(f, 0, SEEK_END) & 4095;
           cpu->ram[cpu->r[0xd]+6] = (p & 0x0000ff00) >> 8;
           cpu->ram[cpu->r[0xd]+7] = (p & 0x000000ff);
           p = lseek(f, 0, SEEK_SET);
           if (cpu->r[0x7] & 16) cpu->ram[cpu->r[0xd]+8] |= 2;
           cpu->ram[cpu->r[0xd]+9]  = (f & 0xff000000) >> 24;
           cpu->ram[cpu->r[0xd]+10] = (f & 0x00ff0000) >> 16;
           cpu->ram[cpu->r[0xd]+11] = (f & 0x0000ff00) >> 8;
           cpu->ram[cpu->r[0xd]+12] = (f & 0x000000ff);
           if (cpu->r[0x7] & 4) {
             p = lseek(f, 0, SEEK_END);
             cpu->ram[cpu->r[0xd]+0] = (p & 0xff000000) >> 24;
             cpu->ram[cpu->r[0xd]+1] = (p & 0x00ff0000) >> 16;
             cpu->ram[cpu->r[0xd]+2] = (p & 0x0000ff00) >> 8;
             cpu->ram[cpu->r[0xd]+3] = (p & 0x000000ff);
             }
           else {
             cpu->ram[cpu->r[0xd]+0]  = 0;
             cpu->ram[cpu->r[0xd]+1]  = 0;
             cpu->ram[cpu->r[0xd]+2]  = 0;
             cpu->ram[cpu->r[0xd]+3]  = 0;
             }
           checkEOF(cpu);
           cpu->df = 0;
           sret(cpu);
           return;
           break;
      case 0x0309:                                                         // o_read
           if ((cpu->ram[cpu->r[0xd]+8] & 0x8) == 0) {
             cpu->df = 1;
             cpu->d = 2;
             sret(cpu);
             return;
             }
           f = cpu->ram[cpu->r[0xd]+9] << 24;
           f |= cpu->ram[cpu->r[0xd]+10] << 16;
           f |= cpu->ram[cpu->r[0xd]+11] << 8;
           f |= cpu->ram[cpu->r[0xd]+12];
           cpu->r[0xc] = read(f, &(cpu->ram[cpu->r[0xf]]), cpu->r[0xc]);
           p = lseek(f, 0, SEEK_CUR);
           cpu->ram[cpu->r[0xd]+0] = (p & 0xff000000) >> 24;
           cpu->ram[cpu->r[0xd]+1] = (p & 0x00ff0000) >> 16;
           cpu->ram[cpu->r[0xd]+2] = (p & 0x0000ff00) >> 8;
           cpu->ram[cpu->r[0xd]+3] = (p & 0x000000ff);
           cpu->df = 0;
           checkEOF(cpu);
           sret(cpu);
           return;
           break;
      case 0x0312:                                                         // o_close
           if ((cpu->ram[cpu->r[0xd]+8] & 0x8) == 0) {
             cpu->df = 1;
             cpu->d = 2;
             sret(cpu);
             return;
             }
           f = cpu->ram[cpu->r[0xd]+9] << 24;
           f |= cpu->ram[cpu->r[0xd]+10] << 16;
           f |= cpu->ram[cpu->r[0xd]+11] << 8;
           f |= cpu->ram[cpu->r[0xd]+12];
           close(f);
           cpu->ram[cpu->r[0xd]+8] = 0;
           cpu->df = 0;
           sret(cpu);
           return;
           break;
      case 0x030c:                                                         // o_write
           if ((cpu->ram[cpu->r[0xd]+8] & 0x8) == 0) {
             cpu->df = 1;
             cpu->d = 2;
             sret(cpu);
             return;
             }
           f = cpu->ram[cpu->r[0xd]+9] << 24;
           f |= cpu->ram[cpu->r[0xd]+10] << 16;
           f |= cpu->ram[cpu->r[0xd]+11] << 8;
           f |= cpu->ram[cpu->r[0xd]+12];
           cpu->r[0xc] = write(f, &(cpu->ram[cpu->r[0xf]]), cpu->r[0xc]);
           p = lseek(f, 0, SEEK_CUR);
           cpu->ram[cpu->r[0xd]+0] = (p & 0xff000000) >> 24;
           cpu->ram[cpu->r[0xd]+1] = (p & 0x00ff0000) >> 16;
           cpu->ram[cpu->r[0xd]+2] = (p & 0x0000ff00) >> 8;
           cpu->ram[cpu->r[0xd]+3] = (p & 0x000000ff);
           cpu->df = 0;
           checkEOF(cpu);
           sret(cpu);
           return;
           break;
      case 0x030f:                                                         // o_seek
           if ((cpu->ram[cpu->r[0xd]+8] & 0x8) == 0) {
             cpu->df = 1;
             cpu->d = 2;
             sret(cpu);
             return;
             }
           f = cpu->ram[cpu->r[0xd]+9] << 24;
           f |= cpu->ram[cpu->r[0xd]+10] << 16;
           f |= cpu->ram[cpu->r[0xd]+11] << 8;
           f |= cpu->ram[cpu->r[0xd]+12];
           p = (cpu->r[8] << 16) | cpu->r[7];
           if ((cpu->r[0xc] &0xff) == 0) p = lseek(f, p, SEEK_SET);
           if ((cpu->r[0xc] &0xff) == 1) p = lseek(f, p, SEEK_CUR);
           if ((cpu->r[0xc] &0xff) == 2) p = lseek(f, p, SEEK_END);
           cpu->ram[cpu->r[0xd]+0] = (p & 0xff000000) >> 24;
           cpu->ram[cpu->r[0xd]+1] = (p & 0x00ff0000) >> 16;
           cpu->ram[cpu->r[0xd]+2] = (p & 0x0000ff00) >> 8;
           cpu->ram[cpu->r[0xd]+3] = (p & 0x000000ff);
           cpu->r[8] = (p &0xffff0000) >> 16;
           cpu->r[7] = (p & 0x0000ffff);
           cpu->df = 0;
           checkEOF(cpu);
           sret(cpu);
           return;
           break;
      case 0x031b:                                                         // o_rename
           i = 0;
           while (cpu->ram[cpu->r[0xf]] != 0)
             buffer[i++] = cpu->ram[cpu->r[0xf]++];
           buffer[i] = 0;
           i = 0;
           while (cpu->ram[cpu->r[0xc]] != 0)
             name2[i++] = cpu->ram[cpu->r[0xc]++];
           name2[i] = 0;
           i = rename(buffer,name2);
           cpu->df = (i == 0) ? 0 : 1;
           sret(cpu);
           return;
           break;
      case 0x0318:                                                         // o_delete
           i = 0;
           while (cpu->ram[cpu->r[0xf]] != 0)
             buffer[i++] = cpu->ram[cpu->r[0xf]++];
           buffer[i] = 0;
           i = unlink(buffer);
           cpu->df = (i == 0) ? 0 : 1;
           sret(cpu);
           return;
           break;
      case 0x031e:                                                         // o_exec
           cpu->df = 1;
           sret(cpu);
           return;
           break;
      case 0x0315:                                                         // o_opendir
           cpu->df = 1;
           sret(cpu);
           return;
           break;
      case 0x0321:                                                         // o_mkdir
           i = 0;
           while (cpu->ram[cpu->r[0xf]] != 0)
             buffer[i++] = cpu->ram[cpu->r[0xf]++];
           buffer[i] = 0;
           i = mkdir(buffer,0755);
           cpu->df = (i == 0) ? 0 : 1;
           sret(cpu);
           return;
           break;
      case 0x0324:                                                         // o_chdir
           i = 0;
           while (cpu->ram[cpu->r[0xf]] != 0)
             buffer[i++] = cpu->ram[cpu->r[0xf]++];
           buffer[i] = 0;
           i = chdir(buffer);
           cpu->df = (i == 0) ? 0 : 1;
           sret(cpu);
           return;
           break;
      case 0x0327:                                                         // o_rmdir
           i = 0;
           while (cpu->ram[cpu->r[0xf]] != 0)
             buffer[i++] = cpu->ram[cpu->r[0xf]++];
           buffer[i] = 0;
           i = rmdir(buffer);
           cpu->df = (i == 0) ? 0 : 1;
           sret(cpu);
           return;
           break;
      case 0x036c:                                                         // o_alloc
           doAlloc(cpu);
           sret(cpu);
           return;
           break;
      case 0x036f:                                                         // o_dealloc
           doDealloc(cpu);
           sret(cpu);
           return;
           break;
      }
//    if (cpu->r[cpu->p] < 0x2000) {
//printf("Unsupported Elf/OS call: %04x\n",cpu->r[cpu->p]);
//    exit(1);
//      }
    }
  if (useBios) {
    switch (cpu->r[cpu->p]) {
      case 0xf800:                                                          // f_bread
           if (useF800) {
             if (showTrace) strcat(tbuffer, "CALL  F_BREAD");
             read(0,&key,1);
             if (key == 127) key = 8;
             if ((cpu->r[0xe] & 0x0100) != 0) printf("%c",key);
             fflush(stdout);
             cpu->d = key;
             sret(cpu);
             if (showTrace) trace(tbuffer);
             return;
             }
           break;
      case 0xf803:                                                           // f_btype
           if (useF800) {
             if (showTrace) strcat(tbuffer, "CALL  F_BTYPE");
             if (cpu->d == 0x0c) printf("\e[2J\e[1;1H");
               else printf("%c",cpu->d);
             fflush(stdout);
             sret(cpu);
             if (showTrace) trace(tbuffer);
             return;
             }
           break;
      case 0xf809:                                                           // f_utype
           if (useF800) {
             if (showTrace) strcat(tbuffer, "CALL  F_UTYPE");
             if (cpu->d == 0x0c) printf("\e[2J\e[1;1H");
               else printf("%c",cpu->d);
             fflush(stdout);
             sret(cpu);
             if (showTrace) trace(tbuffer);
             return;
             }
           break;
      case 0xf80c:                                                          // f_uread
           if (useF800) {
             if (showTrace) strcat(tbuffer, "CALL  F_UREAD");
             read(0,&key,1);
             if (key == 127) key = 8;
             if ((cpu->r[0xe] & 0x0100) != 0) printf("%c",key);
             fflush(stdout);
             cpu->d = key;
             sret(cpu);
             if (showTrace) trace(tbuffer);
             return;
             }
           break;
      case 0xf812:                                                           // f_usetbd
           if (useF800) {
             if (showTrace) strcat(tbuffer, "CALL  F_USETBD");
             cpu->df = (useUART) ? 0 : 1;
             sret(cpu);
             if (showTrace) trace(tbuffer);
             return;
             }
           break;
      case 0xf815:                                                           // f_gettod
           if (useF800) {
             tim = time(NULL);
             localtime_r(&tim, &dt);
             cpu->ram[cpu->r[0xf]++] = dt.tm_mon + 1;
             cpu->ram[cpu->r[0xf]++] = dt.tm_mday;
             cpu->ram[cpu->r[0xf]++] = dt.tm_year + 1900 - 1972;
             cpu->ram[cpu->r[0xf]++] = dt.tm_hour;
             cpu->ram[cpu->r[0xf]++] = dt.tm_min;
             cpu->ram[cpu->r[0xf]++] = dt.tm_sec;
             cpu->df = 0;
             sret(cpu);
             return; 
             }
           break;
      case 0xf818:                                                           // f_settod
           if (useF800) {
             if (showTrace) strcat(tbuffer, "CALL  F_SETTOD");
             cpu->df = (useRTC) ? 0 : 1;
             sret(cpu);
             if (showTrace) trace(tbuffer);
             return;
             }
           break;
      case 0xf81b:                                                           // f_rdnvr
           if (useF800) {
             if (showTrace) strcat(tbuffer, "CALL  F_RDNVR");
             if (useNVR) {
               for (i=0; i<(cpu->r[0xc] & 0xff); i++)
                 cpu->ram[cpu->r[0xd]+i] = nvr[(cpu->r[0xf]+i+0x0e) & 0x7f];
               cpu->df = 0;
               }
             else
               cpu->df = 1;
             sret(cpu);
             if (showTrace) trace(tbuffer);
             return;
             }
           break;
      case 0xf81e:                                                           // f_wrnvr
           if (useF800) {
             if (showTrace) strcat(tbuffer, "CALL  F_WRNVR");
             if (useNVR) {
               for (i=0; i<(cpu->r[0xc] & 0xff); i++)
                 nvr[(cpu->r[0xf]+i+0x0e) & 0x7f] = cpu->ram[cpu->r[0xd]+i];
               f = open("run02.nvr", O_CREAT | O_TRUNC | O_WRONLY, 0666);
               if (f > 0) {
                 write(f, nvr, 128);
                 close(f);
                 }
               cpu->df = 0;
               }
             else
               cpu->df = 1;
             sret(cpu);
             if (showTrace) trace(tbuffer);
             return;
             }
           break;
      case 0xf82d:                                                           // f_rtctest
           if (useF800) {
             if (showTrace) strcat(tbuffer, "CALL  F_RTCTEST");
             cpu->df = (useRTC) ? 1 : 0;
             if (useNVR) {
               cpu->d = 114;
               }
             else
               cpu->d = 0;
             sret(cpu);
             if (showTrace) trace(tbuffer);
             return;
             }
           break;
      case 0xff3f:                                                           // f_initcall
           if (showTrace) strcat(tbuffer, "CALL  F_INITCALL");
           cpu->r[4] = 0xfa7b;
           cpu->r[5] = 0xfa8d;
           cpu->r[3] = cpu->r[6];
           cpu->p = 3;
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xffe0:
      case 0xfa7b:                                                           // call
           if (showTrace) {
             sprintf(tline, "CALL  SCALL %02X%02X",cpu->ram[cpu->r[3]],cpu->ram[cpu->r[3]+1]);
             strcat(tbuffer, tline);
             }
           cpu->r[14] = (cpu->r[14] & 0xff00) | cpu->d;
           cpu->x = 2;
           cpu->ram[cpu->r[cpu->x]--] = ((cpu->r[6] >> 8) & 0xff);
           cpu->ram[cpu->r[cpu->x]--] = (cpu->r[6] & 0xff);
           cpu->r[6] = cpu->r[3];
           cpu->r[3] = (cpu->ram[cpu->r[6]++] << 8);
           cpu->r[3] |= (cpu->ram[cpu->r[6]++]);
           cpu->p = 3;
           cpu->r[4] = 0xfa7b;
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xfff1:
      case 0xfa8d:                                                           // ret
           if (showTrace) strcat(tbuffer, "CALL  SRET");
           cpu->r[14] = (cpu->r[14] & 0xff00) | cpu->d;
           cpu->x = 2;
           cpu->r[3] = cpu->r[6];
           cpu->r[cpu->x]++;
           cpu->r[6] = (cpu->ram[cpu->r[cpu->x]++]);
           cpu->r[6] |= ((cpu->ram[cpu->r[cpu->x]]) << 8);
           cpu->p = 3;
           cpu->r[5] = 0xfa8d;
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff00:                                                           // f_boot
           ideBoot(cpu);
           return;
           break;
      case 0xff03:                                                           // f_type
           if (showTrace) strcat(tbuffer, "CALL  F_TYPE");
           if (cpu->d == 0x0c) printf("\e[2J\e[1;1H");
             else printf("%c",cpu->d);
           fflush(stdout);
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff06:                                                           // f_read
           if (showTrace) strcat(tbuffer, "CALL  F_READ");
           read(0,&key,1);
           if (key == 127) key = 8;
           if ((cpu->r[0xe] & 0x0100) != 0) printf("%c",key);
           fflush(stdout);
           cpu->d = key;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff09:                                                           // f_msg
           if (showTrace) sprintf(tline,"CALL  F_MSG          ");
           i = cpu->ram[cpu->r[0xf]++];
           while (i != 0) {
             if (showTrace) {
               if (i >= 32) {
                 buffer2[0] = i;
                 strcat(tbuffer,buffer2);
                 }
               else {
                 sprintf(buffer,"<%02x>",i);
                 strcat(tbuffer,buffer);
                 }
               }
             if (i==0xc) printf("\e[2J\e[1;1H");
               else printf("%c",i);
             i = cpu->ram[cpu->r[0xf]++];
             }
           if (showTrace) {
             strcat(tbuffer,"\n");
             trace(tbuffer);
             }
           fflush(stdout);
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff0f:                                                           // f_input
      case 0xff69:                                                           // f_inputl
           if (showTrace) strcat(tbuffer, "CALL  F_INPUT");
           key = 0;
           while (key != 10 && key != 13 && key != 27) {
             read(0,&key,1);
             if (key >= ' ' && key < 127) {
               cpu->ram[cpu->r[0x0f]++] = key;
               printf("%c",key);
               fflush(stdout);
               }
             else if (key == 8 || key == 127) {
               printf("%c %c",8,8);
               fflush(stdout);
               cpu->r[0x0f]--;
               }
             else if (key == 10 || key == 13) {
               cpu->df = 0;
               }
             else if (key == 27) {
               cpu->df = 1;
               }
             }
           cpu->ram[cpu->r[0xf]] = 0;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff12:                                                           // f_strcmp
           if (showTrace) strcat(tbuffer, "CALL  F_STRCMP");
           if (showTrace) trace(tbuffer);
           while (cpu->ram[cpu->r[0xf]] != 0 &&
                  cpu->ram[cpu->r[0xd]] != 0) {
             if (cpu->ram[cpu->r[0xf]] < cpu->ram[cpu->r[0xd]]) {
               cpu->d = 0xff;
               sret(cpu);
               return;
               }
             if (cpu->ram[cpu->r[0xf]] > cpu->ram[cpu->r[0xd]]) {
               cpu->d = 0x01;
               sret(cpu);
               return;
               }
             cpu->r[0xf]++;
             cpu->r[0xd]++;
             }
           if (cpu->ram[cpu->r[0xf]] == cpu->ram[cpu->r[0xd]]) {
             cpu->d = 0;
             sret(cpu);
             return;
             }
           if (cpu->ram[cpu->r[0xf]] == 0) {
             cpu->d = 0xff;
             sret(cpu);
             return;
             }
           cpu->d = 0x01;
           sret(cpu);
           return;
           break;
      case 0xff15:                                                           // f_ltrim
           if (showTrace) strcat(tbuffer, "CALL  F_LTRIM");
           key = cpu->ram[cpu->r[15]];
           while (key > 0 && key <= ' ') key = cpu->ram[++cpu->r[15]];
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff18:                                                           // f_strcpy
           if (showTrace) strcat(tbuffer, "CALL  F_STRCPY");
           i = cpu->ram[cpu->r[0xf]++];
           cpu->ram[cpu->r[0xd]++] = i;
           while (i != 0) {
             i = cpu->ram[cpu->r[0xf]++];
             cpu->ram[cpu->r[0xd]++] = i;
             }
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff1b:                                                           // f_memcpy
           if (showTrace) strcat(tbuffer, "CALL  F_MEMCPY");
           for (w=0; w<cpu->r[0xc]; w++)
             cpu->ram[cpu->r[0xd]++] = cpu->ram[cpu->r[0xf]++];
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff2d:                                                           // f_setbd
           if (showTrace) strcat(tbuffer, "CALL  F_SETBD");
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff30:                                                           // f_mul16
           if (showTrace) strcat(tbuffer, "CALL  F_MUL16");
           w = cpu->r[0xf];
           w *= cpu->r[0xd];
           cpu->r[0xc] = (w & 0xffff0000) >> 16;
           cpu->r[0xb] = w & 0xffff;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff33:                                                           // f_div16
           if (showTrace) strcat(tbuffer, "CALL  F_DIV16");
           cpu->r[0xb] = cpu->r[0xf] / cpu->r[0xd];
           cpu->r[0xf] = cpu->r[0xf] % cpu->r[0xd];
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff36:                                                           // f_idereset
           if (showTrace) strcat(tbuffer, "CALL  F_IDERESET");
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff39:                                                           // f_idewrite
           if (showTrace) strcat(tbuffer, "CALL  F_IDEWRITE");
           ideWrite(cpu);
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff3c:                                                           // f_ideread
           if (showTrace) strcat(tbuffer, "CALL  F_IDEREAD");
           ideRead(cpu);
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff45:                                                           // f_hexin
           if (showTrace) strcat(tbuffer, "CALL  F_HEXIN");
           cpu->r[0xd] = 0;
           while ((cpu->ram[cpu->r[0xf]] >= '0' && cpu->ram[cpu->r[0xf]] <= '9') ||
                  (cpu->ram[cpu->r[0xf]] >= 'a' && cpu->ram[cpu->r[0xf]] <= 'f') ||
                  (cpu->ram[cpu->r[0xf]] >= 'A' && cpu->ram[cpu->r[0xf]] <= 'F')) {
             if (cpu->ram[cpu->r[0xf]] >= '0' && cpu->ram[cpu->r[0xf]] <= '9')
               cpu->r[0xd] = (cpu->r[0xd] << 4) + (cpu->ram[cpu->r[0xf]++] - '0');
             if (cpu->ram[cpu->r[0xf]] >= 'a' && cpu->ram[cpu->r[0xf]] <= 'f')
               cpu->r[0xd] = (cpu->r[0xd] << 4) + 10 + (cpu->ram[cpu->r[0xf]++] - 'a');
             if (cpu->ram[cpu->r[0xf]] >= 'A' && cpu->ram[cpu->r[0xf]] <= 'F')
               cpu->r[0xd] = (cpu->r[0xd] << 4) + 10 + (cpu->ram[cpu->r[0xf]++] - 'A');
             }
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff48:                                                           // f_hexout2
           if (showTrace) strcat(tbuffer, "CALL  F_HEXOUT2");
           sprintf(buffer,"%02X",(cpu->r[0xd] & 0xff));
           for (i=0; i<strlen(buffer); i++)
             cpu->ram[cpu->r[0xf]++] = buffer[i];
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff4b:                                                           // f_hexout4
           if (showTrace) strcat(tbuffer, "CALL  F_HEXOUT4");
           sprintf(buffer,"%04X",cpu->r[0xd]);
           for (i=0; i<strlen(buffer); i++)
             cpu->ram[cpu->r[0xf]++] = buffer[i];
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff4e:                                                           // f_tty
           if (showTrace) strcat(tbuffer, "CALL  F_TTY");
           printf("%c",cpu->d);
           fflush(stdout);
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff57:                                                           // f_freemem
           if (showTrace) strcat(tbuffer, "CALL  F_FREEMEM");
           cpu->r[0xf] = ramEnd;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff5a:                                                           // f_isnum
           if (showTrace) strcat(tbuffer, "CALL  F_ISNUM");
           if (cpu->d >= '0' && cpu->d <= '9') cpu->df = 1;
           else cpu->df = 0;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff5d:                                                           // f_atoi
           if (showTrace) strcat(tbuffer, "CALL  F_ATOI");
           if (showTrace) trace(tbuffer);
           w = cpu->r[15];
           cpu->r[0xd] = 0;
           if (cpu->ram[w] < '0' || cpu->ram[w] > '9') {
             cpu->df = 1;
             sret(cpu);
             return;
             }
           while (cpu->ram[w] >= '0' && cpu->ram[w] <= '9') {
             cpu->r[0xd] = (cpu->r[0xd] * 10) + (cpu->ram[w]-'0');
             w++;
             }
           cpu->r[15] = w;
           cpu->df = 0;
           sret(cpu);
           return;
           break;
      case 0xff60:                                                           // f_uintout
           w = cpu->r[0xd];
           if (showTrace) {
             sprintf(tline,"CALL  F_UINTOUT      %u",w);
             strcat(tbuffer, tline);
             if (showTrace) trace(tbuffer);
             }
           sprintf(buffer,"%d",w);
           for (i=0; i<strlen(buffer); i++)
             cpu->ram[cpu->r[15]++] = buffer[i];
           sret(cpu);
           return;
           break;
      case 0xff63:                                                           // f_intout
           w = cpu->r[0xd];
           if (showTrace) {
             sprintf(tline,"CALL  F_UINTOUT      %d",w);
             strcat(tbuffer, tline);
             if (showTrace) trace(tbuffer);
             }
           if (w & 0x8000) {
             cpu->ram[cpu->r[15]++] = '-';
             w = (w ^ 0xffff) + 1;
             }
           sprintf(buffer,"%d",w);
           for (i=0; i<strlen(buffer); i++)
             cpu->ram[cpu->r[15]++] = buffer[i];
           sret(cpu);
           return;
           break;
      case 0xff66:                                                           // f_inmsg
           if (showTrace) sprintf(tline,"CALL  F_INMSG        ");
           i = cpu->ram[cpu->r[6]++];
           while (i != 0) {
             if (showTrace) {
               if (i >= 32) {
                 buffer2[0] = i;
                 strcat(tbuffer,buffer2);
                 }
               else {
                 sprintf(buffer,"<%02x>",i);
                 strcat(tbuffer,buffer);
                 }
               }
             if (i == 0xc) printf("\e[2J\e[1;1H");
               else printf("%c",i);
             i = cpu->ram[cpu->r[6]++];
             }
           if (showTrace) {
             strcat(tbuffer,"\n");
             trace(tbuffer);
             }
           fflush(stdout);
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff6f:                                                           // f_findtkn
           if (showTrace) strcat(tbuffer, "CALL  F_FINDTKN");
           if (showTrace) trace(tbuffer);
           w = cpu->r[0x7];
           cpu->r[0xd] = 0;
           while (cpu->ram[w] != 0) {
             f = 0xff;
             i = 0;
             while (cpu->ram[w] < 0x80) {
               if (cpu->ram[w] != cpu->ram[cpu->r[0xf]+i]) f = 0;
               w++;
               i++;
               }
             if (f != 0 && (cpu->ram[w] & 0x7f) == cpu->ram[cpu->r[0xf]+i]) {
               cpu->df = 1;
               cpu->r[0xf] += i+1;
               sret(cpu);
               return;
               }
             w++;
             cpu->r[0xd]++;
             }
           cpu->df = 0;
           sret(cpu);
           return;
           break;
      case 0xff72:                                                           // f_isalpha
           if (showTrace) strcat(tbuffer, "CALL  F_ISALPHA");
           if ((cpu->d >= 'a' && cpu->d <= 'z') ||
               (cpu->d >= 'A' && cpu->d <= 'Z')) cpu->df = 1;
           else cpu->df = 0;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff75:                                                           // f_ishex
           if (showTrace) strcat(tbuffer, "CALL  F_ISHEX");
           if ((cpu->d >= 'a' && cpu->d <= 'f') ||
               (cpu->d >= 'A' && cpu->d <= 'F') ||
               (cpu->d >= '0' && cpu->d <= '9')) cpu->df = 1;
           else cpu->df = 0;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff78:                                                           // f_isalnum
           if (showTrace) strcat(tbuffer, "CALL  F_ISALNUM");
           if ((cpu->d >= 'a' && cpu->d <= 'z') ||
               (cpu->d >= 'A' && cpu->d <= 'Z') ||
               (cpu->d >= '0' && cpu->d <= '9')) cpu->df = 1;
           else cpu->df = 0;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff7b:                                                           // f_idnum
           if (showTrace) strcat(tbuffer, "CALL  F_IDNUM");
           w = cpu->r[15];
           if (cpu->ram[w] < '0' || cpu->ram[w] > '9') {
             cpu->df = 1;
             sret(cpu);
             return;
             }
           w++;
           while (cpu->ram[w] >= '0' && cpu->ram[w] <= '9') w++;
           if (!(cpu->ram[w] >= 'a' && cpu->ram[w] <= 'z') &&
               !(cpu->ram[w] >= 'A' && cpu->ram[w] <= 'Z')) {
             cpu->df = 0;
             cpu->d = 0;
             sret(cpu);
             return;
             }
           while ((cpu->ram[w] >= '0' && cpu->ram[w] <= '9') ||
                  (cpu->ram[w] >= 'a' && cpu->ram[w] <= 'f') ||
                  (cpu->ram[w] >= 'A' && cpu->ram[w] <= 'F')) w++;
           if (!(cpu->ram[w] >= 'a' && cpu->ram[w] <= 'z') &&
               !(cpu->ram[w] >= 'A' && cpu->ram[w] <= 'Z')) {
             cpu->df = 0;
             cpu->d = 1;
             sret(cpu);
             return;
             }
           cpu->df = 1;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff7e:                                                           // f_isterm
           if (showTrace) strcat(tbuffer, "CALL  F_ISALNUM");
           if ((cpu->d >= 'a' && cpu->d <= 'z') ||
               (cpu->d >= 'A' && cpu->d <= 'Z') ||
               (cpu->d >= '0' && cpu->d <= '9')) cpu->df = 0;
           else cpu->df = 1;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff81:                                                          // f_getdev
           if (showTrace) strcat(tbuffer, "CALL  F_GETDEV");
           cpu->r[0xf] = 0x0005;
           if (useRTC) cpu->r[0xf] |= 0x10;
           if (useNVR) cpu->r[0xf] |= 0x20;
           if (useUART) cpu->r[0xf] |= 0x08;
           if (useF800) cpu->r[0xf] |= 0x40;
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return; 
           break;
      case 0xff87:                                                           // f_sdread
           if (showTrace) strcat(tbuffer, "CALL  F_SDREAD");
           cpu->r[8] = (cpu->r[8] & 0x00ff) | 0xe000;
           if (cpu->d) cpu->r[8] |= 0x1000;
           ideRead(cpu);
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff8a:                                                           // f_sdwrite
           if (showTrace) strcat(tbuffer, "CALL  F_SDWRITE");
           if (cpu->d) cpu->r[8] |= 0x1000;
           ideRead(cpu);
           ideWrite(cpu);
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      case 0xff8d:                                                           // f_sdreset
           if (showTrace) strcat(tbuffer, "CALL  F_SDRESET");
           sret(cpu);
           if (showTrace) trace(tbuffer);
           return;
           break;
      }
    if (cpu->r[cpu->p] >= 0xf000) {
      printf("Unsupported BIOS call: %04x\n",cpu->r[cpu->p]);
      if (tcsetattr(0,TCSANOW,&original) != 0) {
        printf("Could not restore terminal attributes\n");
        }
      exit(1);
      }
    }

  i = readMem(cpu, cpu->r[cpu->p]++);
  imap[i]++;
  cpu->n = i & 0xf;
  cpu->i = (i >> 4) & 0xff;
  if (freq != 0) {
    cycles = (cpu->i != 0xc) ? 2 : 3;
    gettimeofday(&tv,NULL);
    st = tv.tv_sec * 1000000 + tv.tv_usec;
    et = st;
    while (et-st < cycles * freq) {
      gettimeofday(&tv,NULL);
      et = tv.tv_sec * 1000000 + tv.tv_usec;
      }
    }
  icount++;
  cycles = 2;
  switch (cpu->i) {
    case 0:                                                                // LDN
         if (cpu->n == 0) {                                                // IDL
           if (showTrace) strcat(tbuffer, "IDL");
           cpu->idle = 1;
           }
         else {
           cpu->d = readMem(cpu, cpu->r[cpu->n]);
           if (showTrace) {
             sprintf(tline,"LDN   R%X             D=%02x [%04x]",cpu->n,cpu->d,cpu->r[cpu->n]);
             strcat(tbuffer, tline);
             }
           }
         break;
    case 1:                                                                // INC
         cpu->r[cpu->n]++;
         if (showTrace) {
           sprintf(tline,"INC   R%X             R%X=%04x",cpu->n,cpu->n,cpu->r[cpu->n]);
           strcat(tbuffer, tline);
           }
         break;
    case 2:                                                                // DEC
         cpu->r[cpu->n]--;
         if (showTrace) {
           sprintf(tline,"DEC   R%X             R%X=%04x",cpu->n,cpu->n,cpu->r[cpu->n]);
           strcat(tbuffer, tline);
           }
         break;
    case 3:
         switch (cpu->n) {
           case 0x00:                                                      // BR
                if (showTrace) {
                  sprintf(tline,"BR    %02X             *",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                 readMem(cpu, cpu->r[cpu->p]);
                break;
           case 0x01:                                                      // BQ
                if (showTrace) {
                  sprintf(tline, "BQ    %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->q)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x02:                                                      // BZ
                if (showTrace) {
                  sprintf(tline, "BZ    %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->d == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x03:                                                      // BDF
                if (showTrace) {
                  sprintf(tline, "BDF   %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->df != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x04:                                                      // B1
                if (showTrace) {
                  sprintf(tline, "B1    %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->ef[0] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x05:                                                      // B2
                if (showTrace) {
                  sprintf(tline, "B2    %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->ef[1] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x06:                                                      // B3
                if (showTrace) {
                  sprintf(tline, "B3    %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->ef[2] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x07:                                                      // B4
                if (showTrace) {
                  sprintf(tline, "B4    %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->ef[3] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x08:                                                      // NBR
                if (showTrace) strcat(tbuffer, "NBR\n");
                cpu->r[cpu->p]++;
                break;
           case 0x09:                                                      // BNQ
                if (showTrace) {
                  sprintf(tline, "BNQ   %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->q == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0a:                                                      // BNZ
                if (showTrace) {
                  sprintf(tline, "BNZ   %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->d != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0b:                                                      // BNF
                if (showTrace) {
                  sprintf(tline, "BNF   %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->df == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0c:                                                      // BN1
                if (showTrace) {
                  sprintf(tline, "BN1   %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->ef[0] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0d:                                                      // BN2
                if (showTrace) {
                  sprintf(tline, "BN2   %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->ef[1] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0e:                                                      // BN3
                if (showTrace) {
                  sprintf(tline, "BN3   %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->ef[2] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0f:                                                      // BN4
                if (showTrace) {
                  sprintf(tline, "BN4   %02X",readMem(cpu, cpu->r[cpu->p]));
                  strcat(tbuffer, tline);
                  }
                if (cpu->ef[3] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   readMem(cpu, cpu->r[cpu->p]);
                else
                  cpu->r[cpu->p]++;
                break;
           }
         break;
    case 4:                                                                // LDA
         cpu->d = readMem(cpu, cpu->r[cpu->n]++);
         if (showTrace) {
           sprintf(tline,"LDA   R%X             D=%02x",cpu->n,cpu->d);
           strcat(tbuffer, tline);
           }
         break;
    case 5:                                                                // STR
         if (showTrace) {
           sprintf(tline,"STR   R%X             [%04x]=%02x",cpu->n,cpu->r[cpu->n], cpu->d);
           strcat(tbuffer, tline);
           }
         writeMem(cpu, cpu->r[cpu->n], cpu->d);
         break;
    case 6:
         switch (cpu->n) {
           case 0x00:                                                      // IRX
                cpu->r[cpu->x]++;
                if (showTrace) {
                  sprintf(tline,"IRX                  R%X=%04x",cpu->x,cpu->r[cpu->x]);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x01:                                                      // OUT
           case 0x02:
           case 0x03:
           case 0x04:
           case 0x05:
           case 0x06:
           case 0x07:
                i = readMem(cpu, cpu->r[cpu->x]++);
                write(out[(cpu->n & 0xf)-1], &i, 1);
                break;
           case 0x08:                                                      // 1805 inst
                if (use1805) cpu1805(cpu);
                break;
           case 0x09:                                                      // OUT
           case 0x0a:
           case 0x0b:
           case 0x0c:
           case 0x0d:
           case 0x0e:
           case 0x0f:
                read(inp[(cpu->n & 0xf)-9], &i, 1);
                writeMem(cpu, cpu->r[cpu->x], i);
                cpu->d = i;
                break;
           }
         break;
    case 7:
         switch (cpu->n) {
           case 0x00:                                                      // RET
                if (showTrace) strcat(tbuffer, "RET");
                i = readMem(cpu, cpu->r[cpu->x]++);
                cpu->p = i & 0xf;
                cpu->x = (i >> 4) & 0xf;
                cpu->ie = 1;
                break;
           case 0x01:                                                      // DIS
                if (showTrace) strcat(tbuffer, "DIS");
                i = readMem(cpu, cpu->r[cpu->x]++);
                cpu->p = i & 0xf;
                cpu->x = (i >> 4) & 0xf;
                cpu->ie = 0;
                break;
           case 0x02:                                                      // LDXA
                cpu->d = readMem(cpu, cpu->r[cpu->x]++);
                if (showTrace) {
                  sprintf(tline,"LDXA                 D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x03:                                                      // STXD
                if (showTrace) {
                  sprintf(tline,"STXD                 [%04x]=%02x",cpu->r[cpu->x], cpu->d);
                  strcat(tbuffer, tline);
                  }
                writeMem(cpu, cpu->r[cpu->x], cpu->d);
                cpu->r[cpu->x]--;
                break;
           case 0x04:                                                      // ADC
                w = cpu->d + readMem(cpu, cpu->r[cpu->x]) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"ADC                  D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x05:                                                      // SDB
                w = readMem(cpu, cpu->r[cpu->x]) + ((~cpu->d) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"SDB                  D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x06:                                                      // SHRC
                i = (cpu->d & 1);
                cpu->d = (cpu->d >> 1) | (cpu->df << 7);
                cpu->df = i;
                if (showTrace) {
                  sprintf(tline,"SHRC                 D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x07:                                                      // SMB
                w = cpu->d + ((~readMem(cpu, cpu->r[cpu->x])) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"SMB                  D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x08:                                                      // SAV
                if (showTrace) strcat(tbuffer, "SAV");
                cpu->ram[cpu->r[cpu->x]] = cpu->t;
                break;
           case 0x09:                                                      // MARK
                if (showTrace) strcat(tbuffer, "MARK");
                cpu->t = (cpu->x << 4) | cpu->p;
                cpu->ram[cpu->r[2]--] = cpu->t;
                cpu->x = cpu->p;
                break;
           case 0x0a:                                                      // REQ
                if (showTrace) strcat(tbuffer, "REQ");
                cpu->q = 0;
                break;
           case 0x0b:                                                      // SEQ
                if (showTrace) strcat(tbuffer, "SEQ");
                cpu->q = 1;
                break;
           case 0x0c:                                                      // ADCI
                w = cpu->d + readMem(cpu, cpu->r[cpu->p]++) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"ADCI  %02x             D=%02x",readMem(cpu, cpu->r[cpu->p]-1),cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0d:                                                      // SDBI
                w = readMem(cpu, cpu->r[cpu->p]++) + ((~cpu->d) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"SDBI  %02x             D=%02x",readMem(cpu, cpu->r[cpu->p]-1),cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0e:                                                      // SHLC
                i = (cpu->d & 0x80) >> 7;
                cpu->d = (cpu->d << 1) | cpu->df;
                cpu->df = i;
                if (showTrace) {
                  sprintf(tline,"SHLC                 D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0f:                                                      // SMBI
                w = cpu->d + ((~readMem(cpu, cpu->r[cpu->p]++)) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"SMBI  %02x             D=%02x",readMem(cpu, cpu->r[cpu->p]-1),cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           }
         break;
    case 8:                                                                // GLO
         cpu->d = cpu->r[cpu->n] & 0xff;
         if (showTrace) {
           sprintf(tline,"GLO   R%x             D=%02x",cpu->n,cpu->d);
           strcat(tbuffer, tline);
           }
         break;
    case 9:                                                                // GHI
         cpu->d = (cpu->r[cpu->n] >> 8) & 0xff;
         if (showTrace) {
           sprintf(tline,"GHI   R%x             D=%02x",cpu->n,cpu->d);
           strcat(tbuffer, tline);
           }
         break;
    case 0x0a:                                                             // PLO
         cpu->r[cpu->n] = (cpu->r[cpu->n] & 0xff00) | cpu->d;
         if (showTrace) {
           sprintf(tline,"PLO   R%X             R%X=%04x",cpu->n,cpu->n,cpu->r[cpu->n]);
           strcat(tbuffer, tline);
           }
         break;
    case 0x0b:                                                             // PHI
         cpu->r[cpu->n] = (cpu->r[cpu->n] & 0x00ff) | (cpu->d << 8);
         if (showTrace) {
           sprintf(tline,"PHI   R%X             R%X=%04x",cpu->n,cpu->n,cpu->r[cpu->n]);
           strcat(tbuffer, tline);
           }
         break;
    case 0x0c:
         cycles++;
         switch (cpu->n) {
           case 0x00:                                                      // LBR
                cpu->r[cpu->p] = (readMem(cpu, cpu->r[cpu->p]) << 8) |
                                  readMem(cpu, cpu->r[cpu->p]+1);
                if (showTrace) {
                  sprintf(tline,"LBR   %04x           *",cpu->r[cpu->p]);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x01:                                                      // LBQ
                if (showTrace) {
                  sprintf(tline,"LBQ   %02X%02X           ",readMem(cpu, cpu->r[cpu->p]),readMem(cpu, cpu->r[cpu->p]+1));
                  }
                if (cpu->q) {
                  cpu->r[cpu->p] = (readMem(cpu, cpu->r[cpu->p]) << 8) |
                                    readMem(cpu, cpu->r[cpu->p]+1);
                  if (showTrace) strcat(tline,"*");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tline,"-");
                  }
                if (showTrace) strcat(tbuffer, tline);
                break;
           case 0x02:                                                      // LBZ
                if (showTrace) {
                  sprintf(tline,"LBZ   %02X%02X           ",readMem(cpu, cpu->r[cpu->p]),readMem(cpu, cpu->r[cpu->p]+1));
                  }
                if (cpu->d == 0) {
                  cpu->r[cpu->p] = (readMem(cpu, cpu->r[cpu->p]) << 8) |
                                    readMem(cpu, cpu->r[cpu->p]+1);
                  if (showTrace) strcat(tline,"*");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tline,"-");
                  }
                if (showTrace) strcat(tbuffer, tline);
                break;
           case 0x03:                                                      // LBDF
                if (showTrace) {
                  sprintf(tline,"LBDF  %02X%02X           ",readMem(cpu, cpu->r[cpu->p]),readMem(cpu, cpu->r[cpu->p]+1));
                  }
                if (cpu->df != 0) {
                  cpu->r[cpu->p] = (readMem(cpu, cpu->r[cpu->p]) << 8) |
                                    readMem(cpu, cpu->r[cpu->p]+1);
                  if (showTrace) strcat(tline,"*");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tline,"-");
                  }
                if (showTrace) strcat(tbuffer, tline);
                break;
           case 0x04:                                                      // NOP
                if (showTrace) strcat(tbuffer, "NOP");
                break;
           case 0x05:                                                      // LSNQ
                if (showTrace) {
                  sprintf(tline, "LSNQ  R%X",cpu->n);
                  strcat(tbuffer, tline);
                  }
                if (cpu->q == 0) cpu->r[cpu->p] += 2;
                break;
           case 0x06:                                                      // LSNZ
                if (showTrace) {
                  sprintf(tline, "LSNZ  R%X",cpu->n);
                  strcat(tbuffer, tline);
                  }
                if (cpu->d != 0) cpu->r[cpu->p] += 2;
                break;
           case 0x07:                                                      // LSNF
                if (showTrace) {
                  sprintf(tline, "LSNF  R%X",cpu->n);
                  strcat(tbuffer, tline);
                  }
                if (cpu->df == 0) cpu->r[cpu->p] += 2;
                break;
           case 0x08:                                                      // NLBR
                if (showTrace) {
                  sprintf(tline, "NLBR  R%X",cpu->n);
                  strcat(tbuffer, tline);
                  }
                cpu->r[cpu->p] += 2;
                break;
           case 0x09:                                                      // LBNQ
                if (showTrace) {
                  sprintf(tline,"LBNQ  %02X%02X           ",readMem(cpu, cpu->r[cpu->p]),readMem(cpu, cpu->r[cpu->p]+1));
                  }
                if (cpu->q == 0) {
                  cpu->r[cpu->p] = (readMem(cpu, cpu->r[cpu->p]) << 8) |
                                    readMem(cpu, cpu->r[cpu->p]+1);
                  if (showTrace) strcat(tline,"*");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tline,"-");
                  }
                if (showTrace) strcat(tbuffer, tline);
                break;
           case 0x0a:                                                      // LBNZ
                if (showTrace) {
                  sprintf(tline,"LBNZ  %02X%02X           ",readMem(cpu, cpu->r[cpu->p]),readMem(cpu, cpu->r[cpu->p]+1));
                  }
                if (cpu->d != 0) {
                  cpu->r[cpu->p] = (readMem(cpu, cpu->r[cpu->p]) << 8) |
                                    readMem(cpu, cpu->r[cpu->p]+1);
                  if (showTrace) strcat(tline,"*");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tline,"-");
                  }
                if (showTrace) strcat(tbuffer, tline);
                break;
           case 0x0b:                                                      // LBNF
                if (showTrace) {
                  sprintf(tline,"LBNF  %02X%02X           ",readMem(cpu, cpu->r[cpu->p]),readMem(cpu, cpu->r[cpu->p]+1));
                  }
                if (cpu->df == 0) {
                  cpu->r[cpu->p] = (readMem(cpu, cpu->r[cpu->p]) << 8) |
                                    readMem(cpu, cpu->r[cpu->p]+1);
                  if (showTrace) strcat(tline,"*");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tline,"-");
                  }
                if (showTrace) strcat(tbuffer, tline);
                break;
           case 0x0c:                                                      // LSIE
                if (showTrace) {
                  sprintf(tline, "LSIE  R%X",cpu->n);
                  strcat(tbuffer, tline);
                  }
                if (cpu->ie != 0) cpu->r[cpu->p] += 2;
                break;
           case 0x0d:                                                      // LSQ
                if (showTrace) {
                  sprintf(tline, "LSQ   R%X",cpu->n);
                  strcat(tbuffer, tline);
                  }
                if (cpu->q != 0) cpu->r[cpu->p] += 2;
                break;
           case 0x0e:                                                      // LSZ
                if (showTrace) {
                  sprintf(tline, "LSZ   R%X",cpu->n);
                  strcat(tbuffer, tline);
                  }
                if (cpu->d == 0) cpu->r[cpu->p] += 2;
                break;
           case 0x0f:                                                      // LSDF
                if (showTrace) {
                  sprintf(tline, "LSDF  R%X",cpu->n);
                  strcat(tbuffer, tline);
                  }
                if (cpu->df != 0) cpu->r[cpu->p] += 2;
                break;
           }
         break;
    case 0x0d:                                                             // SEP
         cpu->p = cpu->n;
         if (showTrace) {
           sprintf(tline,"SEP   R%x             P=%x",cpu->n,cpu->p);
           strcat(tbuffer, tline);
           }
         break;
    case 0x0e:                                                             // SEX
         cpu->x = cpu->n;
         if (showTrace) {
           sprintf(tline,"SEX   R%x             X=%x",cpu->n,cpu->x);
           strcat(tbuffer, tline);
           }
         break;
    case 0x0f:
         switch (cpu->n) {
           case 0x00:                                                      // LDX
                cpu->d = readMem(cpu, cpu->r[cpu->x]);
                if (showTrace) {
                  sprintf(tline,"LDX                  D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x01:                                                      // OR
                cpu->d |= readMem(cpu, cpu->r[cpu->x]);
                if (showTrace) {
                  sprintf(tline,"OR                   D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x02:                                                      // AND
                cpu->d &= readMem(cpu, cpu->r[cpu->x]);
                if (showTrace) {
                  sprintf(tline,"AND                  D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x03:                                                      // XOR
                cpu->d ^= readMem(cpu, cpu->r[cpu->x]);
                if (showTrace) {
                  sprintf(tline,"XOR                  D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x04:                                                      // ADD
                w = cpu->d + readMem(cpu, cpu->r[cpu->x]);
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"ADD                  D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x05:                                                      // SD
                w = readMem(cpu, cpu->r[cpu->x]) + ((~cpu->d) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"SD                   D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x06:                                                      // SHR
                i = (cpu->d & 1);
                cpu->d = (cpu->d >> 1);
                cpu->df = i;
                if (showTrace) {
                  sprintf(tline,"SHR                  D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x07:                                                      // SM
                w = cpu->d + ((~readMem(cpu, cpu->r[cpu->x])) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"SM                   D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x08:                                                      // LDI
                cpu->d = readMem(cpu, cpu->r[cpu->p]++);
                if (showTrace) {
                  sprintf(tline,"LDI   %02x             D=%02x",cpu->d,cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x09:                                                      // ORI
                cpu->d |= readMem(cpu, cpu->r[cpu->p]++);
                if (showTrace) {
                  sprintf(tline,"ORI   %02x             D=%02x",readMem(cpu, cpu->r[cpu->p]-1),cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0a:                                                      // ANI
                cpu->d &= readMem(cpu, cpu->r[cpu->p]++);
                if (showTrace) {
                  sprintf(tline,"ANI   %02x             D=%02x",readMem(cpu, cpu->r[cpu->p]-1),cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0b:                                                      // XRI
                cpu->d ^= readMem(cpu, cpu->r[cpu->p]++);
                if (showTrace) {
                  sprintf(tline,"XRI   %02x             D=%02x",readMem(cpu, cpu->r[cpu->p]-1),cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0c:                                                      // ADI
                w = cpu->d + readMem(cpu, cpu->r[cpu->p]++);
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"ADI   %02x             D=%02x",readMem(cpu, cpu->r[cpu->p]-1),cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0d:                                                      // SDI
                w = readMem(cpu, cpu->r[cpu->p]++) + ((~cpu->d) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"SDI   %02x             D=%02x",readMem(cpu, cpu->r[cpu->p]-1),cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0e:                                                      // SHL
                i = (cpu->d & 0x80) >> 7;
                cpu->d = (cpu->d << 1);
                cpu->df = i;
                if (showTrace) {
                  sprintf(tline,"SHL                  D=%02x",cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           case 0x0f:                                                      // SMI
                w = cpu->d + ((~readMem(cpu, cpu->r[cpu->p]++)) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tline,"SMI   %02x             D=%02x",readMem(cpu, cpu->r[cpu->p]-1),cpu->d);
                  strcat(tbuffer, tline);
                  }
                break;
           }
         break;
    }
  if (showTrace) trace(tbuffer);
  if (use1805 && cpu->crunning && cpu->cmode == T_TIMER) {
    while (cycles > 0) {
      cycles--;
      if (cpu->cpre == 0) cpu->cpre = 31;
      else {
        cpu->cpre--;
        if (cpu->cpre == 0) {
          if (cpu->cntr == 0) cpu->cntr = cpu->ch;
          else {
            cpu->cntr--;
            if (cpu->cntr == 0) {
              cpu->ci = 0xff;
              }
            }
          }
        }
      }
    }
  if (use1805 && cpu->ci != 0 && cpu->ie != 0) cpuIntr(cpu, 'C');
  }

