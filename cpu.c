#include "header.h"


void cpuReset(CPU *cpu) {
  cpu->p = 0;
  cpu->x = 0;
  cpu->r[0] = 0;
  cpu->ie = 0;
  cpu->idle = 0;
  cpu->ef[0] = 0;
  cpu->ef[1] = 0;
  cpu->ef[2] = 0;
  cpu->ef[3] = 0;
  }

void cpuCycle(CPU *cpu) {
  byte i;
  byte key;
  word w;
  switch (cpu->r[cpu->p]) {
    case 0xff3f:                                                           // f_initcall
         cpu->r[4] = 0xfa7b;
         cpu->r[5] = 0xfa8d;
         cpu->r[3] = cpu->r[6];
         return;
         break;
    case 0xfa7b:                                                           // call
         cpu->ram[cpu->r[cpu->x]--] = ((cpu->r[6] >> 8) & 0xff);
         cpu->ram[cpu->r[cpu->x]--] = (cpu->r[6] & 0xff);
         cpu->r[6] = cpu->r[3];
         cpu->r[3] = (cpu->ram[cpu->r[6]++] << 8);
         cpu->r[3] |= (cpu->ram[cpu->r[6]++]);
         return;
         break;
    case 0xfa8d:                                                           // ret
         cpu->r[3] = cpu->r[6];
         cpu->r[cpu->x]++;
         cpu->r[6] = ((cpu->ram[cpu->r[cpu->x]++]) << 8);
         cpu->r[6] |= (cpu->ram[cpu->r[cpu->x]]);
         return;
         break;
    case 0xff03:                                                           // f_type
         if (cpu->d == 0x0c) printf("\e[2J");
           else printf("%c",cpu->d);
         fflush(stdout);
         cpu->r[3] = cpu->r[6];
         cpu->r[cpu->x]++;
         cpu->r[6] = ((cpu->ram[cpu->r[cpu->x]++]) << 8);
         cpu->r[6] |= (cpu->ram[cpu->r[cpu->x]]);
         return;
         break;
    case 0xff06:                                                           // f_read
         read(0,&key,1);
         if (key == 127) key = 8;
         printf("%c",key);
         fflush(stdout);
         cpu->d = key;
         cpu->r[3] = cpu->r[6];
         cpu->r[cpu->x]++;
         cpu->r[6] = ((cpu->ram[cpu->r[cpu->x]++]) << 8);
         cpu->r[6] |= (cpu->ram[cpu->r[cpu->x]]);
         return;
         break;
    case 0xff0f:                                                           // f_input
    case 0xff69:                                                           // f_inputl
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
else printf("<%02x>\n",key);
           }
         cpu->ram[cpu->r[0xf]] = 0;
         cpu->r[3] = cpu->r[6];
         cpu->r[cpu->x]++;
         cpu->r[6] = ((cpu->ram[cpu->r[cpu->x]++]) << 8);
         cpu->r[6] |= (cpu->ram[cpu->r[cpu->x]]);
         return;
         break;
    case 0xff4e:                                                           // f_tty
         printf("%c",cpu->d);
         fflush(stdout);
         cpu->r[3] = cpu->r[6];
         cpu->r[cpu->x]++;
         cpu->r[6] = ((cpu->ram[cpu->r[cpu->x]++]) << 8);
         cpu->r[6] |= (cpu->ram[cpu->r[cpu->x]]);
         return;
         break;
    case 0xff09:                                                           // f_msg
         i = cpu->ram[cpu->r[0xf]++];
         while (i != 0) {
           printf("%c",i);
           i = cpu->ram[cpu->r[0xf]++];
           }
         fflush(stdout);
         cpu->r[3] = cpu->r[6];
         cpu->r[cpu->x]++;
         cpu->r[6] = ((cpu->ram[cpu->r[cpu->x]++]) << 8);
         cpu->r[6] |= (cpu->ram[cpu->r[cpu->x]]);
         return;
         break;
    case 0xff66:                                                           // f_inmsg
         i = cpu->ram[cpu->r[6]++];
         while (i != 0) {
           printf("%c",i);
           i = cpu->ram[cpu->r[6]++];
           }
         fflush(stdout);
         cpu->r[3] = cpu->r[6];
         cpu->r[cpu->x]++;
         cpu->r[6] = ((cpu->ram[cpu->r[cpu->x]++]) << 8);
         cpu->r[6] |= (cpu->ram[cpu->r[cpu->x]]);
         return;
         break;
    case 0xff2d:                                                           // f_setbd
         cpu->r[3] = cpu->r[6];
         cpu->r[cpu->x]++;
         cpu->r[6] = ((cpu->ram[cpu->r[cpu->x]++]) << 8);
         cpu->r[6] |= (cpu->ram[cpu->r[cpu->x]]);
         return;
         break;
    }
  i = cpu->ram[cpu->r[cpu->p]++];
  cpu->n = i & 0xf;
  cpu->i = (i >> 4) & 0xff;
  switch (cpu->i) {
    case 0:                                                                // LDN
         if (cpu->n == 0) cpu->idle = 1;                                   // IDL
         else cpu->d = cpu->ram[cpu->r[cpu->n]];
         break;
    case 1:                                                                // INC
         cpu->r[cpu->n]++;
         break;
    case 2:                                                                // DEC
         cpu->r[cpu->n]--;
         break;
    case 3:
         switch (cpu->n) {
           case 0x00:                                                      // BR
                cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                 cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x01:                                                      // BQ
                if (cpu->q)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x02:                                                      // BZ
                if (cpu->d == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x03:                                                      // BDF
                if (cpu->df != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x04:                                                      // B1
                if (cpu->ef[0] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x05:                                                      // B2
                if (cpu->ef[1] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x06:                                                      // B3
                if (cpu->ef[2] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x07:                                                      // B4
                if (cpu->ef[3] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x08:                                                      // NBR
                cpu->r[cpu->p]++;
                break;
           case 0x09:                                                      // BNQ
                if (cpu->q == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x0a:                                                      // BNZ
                if (cpu->d != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x0b:                                                      // BNF
                if (cpu->df == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x0c:                                                      // BN1
                if (cpu->ef[0] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x0d:                                                      // BN2
                if (cpu->ef[1] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x0e:                                                      // BN3
                if (cpu->ef[2] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x0f:                                                      // BN4
                if (cpu->ef[3] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                break;
           }
         break;
    case 4:                                                                // LDA
         cpu->d = cpu->ram[cpu->r[cpu->n]++];
         break;
    case 5:                                                                // STR
         cpu->ram[cpu->r[cpu->n]] = cpu->d;
         break;
    case 6:
         switch (cpu->n) {
           case 0x00:                                                      // IRX
                cpu->r[cpu->x]++;
                break;
           }
         break;
    case 7:
         switch (cpu->n) {
           case 0x00:                                                      // RET
                i = cpu->ram[cpu->r[cpu->x]++];
                cpu->p = i & 0xf;
                cpu->x = (i >> 4) & 0xf;
                cpu->ie = 1;
                break;
           case 0x01:                                                      // DIS
                i = cpu->ram[cpu->r[cpu->x]++];
                cpu->p = i & 0xf;
                cpu->x = (i >> 4) & 0xf;
                cpu->ie = 0;
                break;
           case 0x02:                                                      // LDXA
                cpu->d = cpu->ram[cpu->r[cpu->x]++];
                break;
           case 0x03:                                                      // STXD
                cpu->ram[cpu->r[cpu->x]--] = cpu->d;
                break;
           case 0x04:                                                      // ADC
                w = cpu->d + cpu->ram[cpu->r[cpu->x]] + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           case 0x05:                                                      // SDB
                w = cpu->ram[cpu->r[cpu->x]] + ((~cpu->d) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           case 0x06:                                                      // SHRC
                i = (cpu->d & 1);
                cpu->d = (cpu->d >> 1) | (cpu->df << 7);
                cpu->df = i;
                break;
           case 0x07:                                                      // SMB
                w = cpu->d + ((~cpu->ram[cpu->r[cpu->x]]) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           case 0x08:                                                      // SAV
                cpu->ram[cpu->r[cpu->x]] = cpu->t;
                break;
           case 0x09:                                                      // MARK
                cpu->t = (cpu->x << 4) | cpu->p;
                cpu->ram[cpu->r[2]--] = cpu->t;
                cpu->x = cpu->p;
                break;
           case 0x0a:                                                      // REQ
                cpu->q = 0;
                break;
           case 0x0b:                                                      // SEQ
                cpu->q = 1;
                break;
           case 0x0c:                                                      // ADCI
                w = cpu->d + cpu->ram[cpu->r[cpu->p]++] + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           case 0x0d:                                                      // SDBI
                w = cpu->ram[cpu->r[cpu->p]++] + ((~cpu->d) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           case 0x0e:                                                      // SHLC
                i = (cpu->d & 0x80) >> 7;
                cpu->d = (cpu->d << 1) | cpu->df;
                cpu->df = i;
                break;
           case 0x0f:                                                      // SMBI
                w = cpu->d + ((~cpu->ram[cpu->r[cpu->p]++]) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           }
         break;
    case 8:                                                                // GLO
         cpu->d = cpu->r[cpu->n] & 0xff;
         break;
    case 9:                                                                // GHI
         cpu->d = (cpu->r[cpu->n] >> 8) & 0xff;
         break;
    case 0x0a:                                                             // PLO
         cpu->r[cpu->n] = (cpu->r[cpu->n] & 0xff00) | cpu->d;
         break;
    case 0x0b:                                                             // PHI
         cpu->r[cpu->n] = (cpu->r[cpu->n] & 0x00ff) | (cpu->d << 8);
         break;
    case 0x0c:
         switch (cpu->n) {
           case 0x00:                                                      // LBR
                cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                  cpu->ram[cpu->r[cpu->p]+1];
                break;
           case 0x01:                                                      // LBQ
                if (cpu->q)
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                else
                  cpu->r[cpu->p] += 2;
                break;
           case 0x02:                                                      // LBZ
                if (cpu->d == 0)
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                else
                  cpu->r[cpu->p] += 2;
                break;
           case 0x03:                                                      // LBDF
                if (cpu->df != 0)
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                else
                  cpu->r[cpu->p] += 2;
                break;
           case 0x04:                                                      // NOP
                break;
           case 0x05:                                                      // LSNQ
                if (cpu->q == 0) cpu->r[cpu->p] += 2;
                break;
           case 0x06:                                                      // LSNZ
                if (cpu->d != 0) cpu->r[cpu->p] += 2;
                break;
           case 0x07:                                                      // LSNF
                if (cpu->df == 0) cpu->r[cpu->p] += 2;
                break;
           case 0x08:                                                      // NLBR
                cpu->r[cpu->p] += 2;
                break;
           case 0x09:                                                      // LBNQ
                if (cpu->q == 0)
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                else
                  cpu->r[cpu->p] += 2;
                break;
           case 0x0a:                                                      // LBNZ
                if (cpu->d != 0)
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                else
                  cpu->r[cpu->p] += 2;
                break;
           case 0x0b:                                                      // LBNF
                if (cpu->df == 0)
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                else
                  cpu->r[cpu->p] += 2;
                break;
           case 0x0c:                                                      // LSIE
                if (cpu->ie != 0) cpu->r[cpu->p] += 2;
                break;
           case 0x0d:                                                      // LSQ
                if (cpu->q != 0) cpu->r[cpu->p] += 2;
                break;
           case 0x0e:                                                      // LSZ
                if (cpu->d == 0) cpu->r[cpu->p] += 2;
                break;
           case 0x0f:                                                      // LSDF
                if (cpu->df != 0) cpu->r[cpu->p] += 2;
                break;
           }
         break;
    case 0x0d:                                                             // SEP
         cpu->p = cpu->n;
         break;
    case 0x0e:                                                             // SEX
         cpu->x = cpu->n;
         break;
    case 0x0f:
         switch (cpu->n) {
           case 0x00:                                                      // LDX
                cpu->d = cpu->ram[cpu->r[cpu->x]];
                break;
           case 0x01:                                                      // OR
                cpu->d |= cpu->ram[cpu->r[cpu->x]];
                break;
           case 0x02:                                                      // AND
                cpu->d &= cpu->ram[cpu->r[cpu->x]];
                break;
           case 0x03:                                                      // XOR
                cpu->d ^= cpu->ram[cpu->r[cpu->x]];
                break;
           case 0x04:                                                      // ADD
                w = cpu->d + cpu->ram[cpu->r[cpu->x]];
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           case 0x05:                                                      // SD
                w = cpu->ram[cpu->r[cpu->x]] + ((~cpu->d) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           case 0x06:                                                      // SHR
                i = (cpu->d & 1);
                cpu->d = (cpu->d >> 1);
                cpu->df = i;
                break;
           case 0x07:                                                      // SM
                w = cpu->d + ((~cpu->ram[cpu->r[cpu->x]]) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           case 0x08:                                                      // LDI
                cpu->d = cpu->ram[cpu->r[cpu->p]++];
                break;
           case 0x09:                                                      // ORI
                cpu->d |= cpu->ram[cpu->r[cpu->p]++];
                break;
           case 0x0a:                                                      // ANI
                cpu->d &= cpu->ram[cpu->r[cpu->p]++];
                break;
           case 0x0b:                                                      // XRI
                cpu->d ^= cpu->ram[cpu->r[cpu->p]++];
                break;
           case 0x0c:                                                      // ADI
                w = cpu->d + cpu->ram[cpu->r[cpu->p]++];
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           case 0x0d:                                                      // SDI
                w = cpu->ram[cpu->r[cpu->p]++] + ((~cpu->d) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           case 0x0e:                                                      // SHL
                i = (cpu->d & 0x80) >> 7;
                cpu->d = (cpu->d << 1);
                cpu->df = i;
                break;
           case 0x0f:                                                      // SMI
                w = cpu->d + ((~cpu->ram[cpu->r[cpu->p]++]) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                break;
           }
         break;
    }
  }

