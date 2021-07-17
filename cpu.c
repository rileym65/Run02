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

void sret(CPU *cpu) {
  cpu->r[3] = cpu->r[6];
  cpu->x = 2;
  cpu->r[cpu->x]++;
  cpu->r[6] = (cpu->ram[cpu->r[cpu->x]++]);
  cpu->r[6] |= ((cpu->ram[cpu->r[cpu->x]]) << 8);
  cpu->p = 3;
  cpu->r[5] = 0xfa8d;
  }

void cpuCycle(CPU *cpu) {
  byte i;
  word d;
  byte key;
  int  f;
  int  flags;
  int  p;
  char buffer[32];
  char buffer2[2];
  char tbuffer[256];
  word w;
  if (showTrace) {
    sprintf(tbuffer,"R%x:[%04x] ",cpu->p,cpu->r[cpu->p],cpu->ram[cpu->r[cpu->p]]);
    trace(tbuffer);
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
           if (cpu->r[0xc] == 0) p = lseek(f, p, SEEK_SET);
           if (cpu->r[0xc] == 1) p = lseek(f, p, SEEK_CUR);
           if (cpu->r[0xc] == 2) p = lseek(f, p, SEEK_END);
           cpu->ram[cpu->r[0xd]+0] = (p & 0xff000000) >> 24;
           cpu->ram[cpu->r[0xd]+1] = (p & 0x00ff0000) >> 16;
           cpu->ram[cpu->r[0xd]+2] = (p & 0x0000ff00) >> 8;
           cpu->ram[cpu->r[0xd]+3] = (p & 0x000000ff);
           cpu->r[8] = (p &0xffff0000) >> 16;
           cpu->r[7] = (p & 0x0000ffff);
           cpu->df = 0;
           sret(cpu);
           return;
           break;
      case 0x031b:                                                         // o_rename
           break;
      case 0x0318:                                                         // o_delete
           break;
      case 0x031e:                                                         // o_exec
           break;
      case 0x0315:                                                         // o_opendir
           break;
      case 0x0321:                                                         // o_mkdir
           break;
      case 0x0324:                                                         // o_chdir
           break;
      case 0x0327:                                                         // o_rmdir
           break;
      }
//    if (cpu->r[cpu->p] < 0x2000) {
//printf("Unsupported Elf/OS call: %04x\n",cpu->r[cpu->p]);
//    exit(1);
//      }
    }
  switch (cpu->r[cpu->p]) {
    case 0xff3f:                                                           // f_initcall
         if (showTrace) trace("CALL  F_INITCALL\n");
         cpu->r[4] = 0xfa7b;
         cpu->r[5] = 0xfa8d;
         cpu->r[3] = cpu->r[6];
         return;
         break;
    case 0xfa7b:                                                           // call
         if (showTrace) printf("CALL  SCALL %02X%02X\n",cpu->ram[cpu->r[3]],cpu->ram[cpu->r[3]+1]);
         cpu->x = 2;
         cpu->ram[cpu->r[cpu->x]--] = ((cpu->r[6] >> 8) & 0xff);
         cpu->ram[cpu->r[cpu->x]--] = (cpu->r[6] & 0xff);
         cpu->r[6] = cpu->r[3];
         cpu->r[3] = (cpu->ram[cpu->r[6]++] << 8);
         cpu->r[3] |= (cpu->ram[cpu->r[6]++]);
         cpu->p = 3;
         cpu->r[4] = 0xfa7b;
         return;
         break;
    case 0xfa8d:                                                           // ret
         if (showTrace) trace("CALL  SRET\n");
         cpu->x = 2;
         cpu->r[3] = cpu->r[6];
         cpu->r[cpu->x]++;
         cpu->r[6] = (cpu->ram[cpu->r[cpu->x]++]);
         cpu->r[6] |= ((cpu->ram[cpu->r[cpu->x]]) << 8);
         cpu->p = 3;
         cpu->r[5] = 0xfa8d;
         return;
         break;
    case 0xff03:                                                           // f_type
         if (showTrace) trace("CALL  F_TYPE\n");
         if (cpu->d == 0x0c) printf("\e[2J");
           else printf("%c",cpu->d);
         fflush(stdout);
         sret(cpu);
         return;
         break;
    case 0xff06:                                                           // f_read
         if (showTrace) trace("CALL  F_READ\n");
         read(0,&key,1);
         if (key == 127) key = 8;
         printf("%c",key);
         fflush(stdout);
         cpu->d = key;
         sret(cpu);
         return;
         break;
    case 0xff09:                                                           // f_msg
         if (showTrace) sprintf(tbuffer,"CALL  F_MSG          ");
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
           printf("%c",i);
           i = cpu->ram[cpu->r[0xf]++];
           }
         if (showTrace) {
           strcat(tbuffer,"\n");
           trace(tbuffer);
           }
         fflush(stdout);
         sret(cpu);
         return;
         break;
    case 0xff0f:                                                           // f_input
    case 0xff69:                                                           // f_inputl
         if (showTrace) trace("CALL  F_INPUT\n");
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
         return;
         break;
    case 0xff15:                                                           // f_ltrim
         if (showTrace) trace("CALL  F_LTRIM\n");
         key = cpu->ram[cpu->r[15]];
         while (key > 0 && key <= ' ') key = cpu->ram[++cpu->r[15]];
         sret(cpu);
         return;
         break;
    case 0xff18:                                                           // f_strcpy
         if (showTrace) trace("CALL  F_STRCPY\n");
         i = cpu->ram[cpu->r[0xf]++];
         cpu->ram[cpu->r[0xd]++] = i;
         while (i != 0) {
           i = cpu->ram[cpu->r[0xf]++];
           cpu->ram[cpu->r[0xd]++] = i;
           }
         sret(cpu);
         return;
         break;
    case 0xff1b:                                                           // f_memcpy
         if (showTrace) trace("CALL  F_MEMCPY\n");
         for (w=0; w<cpu->r[0xc]; w++)
           cpu->ram[cpu->r[0xd]++] = cpu->ram[cpu->r[0xf]++];
         sret(cpu);
         return;
         break;
    case 0xff2d:                                                           // f_setbd
         if (showTrace) trace("CALL  F_SETBD\n");
         sret(cpu);
         return;
         break;
    case 0xff30:                                                           // f_mul16
         if (showTrace) trace("CALL  F_MUL16\n");
         w = cpu->r[0xf];
         w *= cpu->r[0xd];
         cpu->r[0xc] = (w & 0xffff0000) >> 16;
         cpu->r[0xb] = w & 0xffff;
         sret(cpu);
         return;
         break;
    case 0xff33:                                                           // f_div16
         if (showTrace) trace("CALL  F_DIV16\n");
         cpu->r[0xb] = cpu->r[0xf] / cpu->r[0xd];
         cpu->r[0xf] = cpu->r[0xf] % cpu->r[0xd];
         sret(cpu);
         return;
         break;
    case 0xff45:                                                           // f_hexin
         if (showTrace) trace("CALL  F_HEXIN\n");
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
         return;
         break;
    case 0xff48:                                                           // f_hexout2
         if (showTrace) trace("CALL  F_HEXOUT2\n");
         sprintf(buffer,"%02x",(cpu->r[0xd] & 0xff));
         for (i=0; i<strlen(buffer); i++)
           cpu->ram[cpu->r[0xf]++] = buffer[i];
         sret(cpu);
         return;
         break;
    case 0xff4b:                                                           // f_hexout4
         if (showTrace) trace("CALL  F_HEXOUT4\n");
         sprintf(buffer,"%04x",cpu->r[0xd]);
         for (i=0; i<strlen(buffer); i++)
           cpu->ram[cpu->r[0xf]++] = buffer[i];
         sret(cpu);
         return;
         break;
    case 0xff4e:                                                           // f_tty
         if (showTrace) trace("CALL  F_TTY\n");
         printf("%c",cpu->d);
         fflush(stdout);
         sret(cpu);
         return;
         break;
    case 0xff57:                                                           // f_freemem
         if (showTrace) trace("CALL  F_FREEMEM\n");
         cpu->r[0xf] = ramEnd;
         sret(cpu);
         return;
         break;
    case 0xff5a:                                                           // f_isnum
         if (showTrace) trace("CALL  F_ISNUM\n");
         if (cpu->d >= '0' && cpu->d <= '9') cpu->df = 1;
         else cpu->df = 0;
         sret(cpu);
         return;
         break;
    case 0xff5d:                                                           // f_atoi
         if (showTrace) trace("CALL  F_ATOI\n");
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
           sprintf(tbuffer,"CALL  F_UINTOUT      %u\n",w);
           trace(tbuffer);
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
           sprintf(tbuffer,"CALL  F_UINTOUT      %d\n",w);
           trace(tbuffer);
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
         if (showTrace) sprintf(tbuffer,"CALL  F_INMSG        ");
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
           printf("%c",i);
           i = cpu->ram[cpu->r[6]++];
           }
         if (showTrace) {
           strcat(tbuffer,"\n");
           trace(tbuffer);
           }
         fflush(stdout);
         sret(cpu);
         return;
         break;
    case 0xff6f:                                                           // f_findtkn
         if (showTrace) trace("CALL  F_FINDTKN\n");
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
         if (showTrace) trace("CALL  F_ISALPHA\n");
         if ((cpu->d >= 'a' && cpu->d <= 'z') ||
             (cpu->d >= 'A' && cpu->d <= 'Z')) cpu->df = 1;
         else cpu->df = 0;
         sret(cpu);
         return;
         break;
    case 0xff75:                                                           // f_ishex
         if (showTrace) trace("CALL  F_ISHEX\n");
         if ((cpu->d >= 'a' && cpu->d <= 'f') ||
             (cpu->d >= 'A' && cpu->d <= 'F') ||
             (cpu->d >= '0' && cpu->d <= '9')) cpu->df = 1;
         else cpu->df = 0;
         sret(cpu);
         return;
         break;
    case 0xff78:                                                           // f_isalnum
         if (showTrace) trace("CALL  F_ISALNUM\n");
         if ((cpu->d >= 'a' && cpu->d <= 'z') ||
             (cpu->d >= 'A' && cpu->d <= 'Z') ||
             (cpu->d >= '0' && cpu->d <= '9')) cpu->df = 1;
         else cpu->df = 0;
         sret(cpu);
         return;
         break;
    case 0xff7b:                                                           // f_idnum
         if (showTrace) trace("CALL  F_IDNUM\n");
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
         return;
         break;
    case 0xff7e:                                                           // f_isterm
         if (showTrace) trace("CALL  F_ISALNUM\n");
         if ((cpu->d >= 'a' && cpu->d <= 'z') ||
             (cpu->d >= 'A' && cpu->d <= 'Z') ||
             (cpu->d >= '0' && cpu->d <= '9')) cpu->df = 0;
         else cpu->df = 1;
         sret(cpu);
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
  i = cpu->ram[cpu->r[cpu->p]++];
  imap[i]++;
  cpu->n = i & 0xf;
  cpu->i = (i >> 4) & 0xff;
  switch (cpu->i) {
    case 0:                                                                // LDN
         if (cpu->n == 0) {                                                // IDL
           if (showTrace) trace("IDL\n");
           cpu->idle = 1;
           }
         else {
           cpu->d = cpu->ram[cpu->r[cpu->n]];
           if (showTrace) {
             sprintf(tbuffer,"LDN   R%X             D=%02x [%04x]\n",cpu->n,cpu->d,cpu->r[cpu->n]);
             trace(tbuffer);
             }
           }
         break;
    case 1:                                                                // INC
         cpu->r[cpu->n]++;
         if (showTrace) {
           sprintf(tbuffer,"INC   R%X             R%X=%04x\n",cpu->n,cpu->n,cpu->r[cpu->n]);
           trace(tbuffer);
           }
         break;
    case 2:                                                                // DEC
         cpu->r[cpu->n]--;
         if (showTrace) {
           sprintf(tbuffer,"DEC   R%X             R%X=%04x\n",cpu->n,cpu->n,cpu->r[cpu->n]);
           trace(tbuffer);
           }
         break;
    case 3:
         switch (cpu->n) {
           case 0x00:                                                      // BR
                if (showTrace) {
                  sprintf(tbuffer,"BR    %02X             *\n",cpu->ram[cpu->r[cpu->p]]);
                  trace(tbuffer);
                  }
                cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                 cpu->ram[cpu->r[cpu->p]];
                break;
           case 0x01:                                                      // BQ
                if (showTrace) printf("BQ    %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->q)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x02:                                                      // BZ
                if (showTrace) printf("BZ    %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->d == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x03:                                                      // BDF
                if (showTrace) printf("BDF   %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->df != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x04:                                                      // B1
                if (showTrace) printf("B1    %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->ef[0] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x05:                                                      // B2
                if (showTrace) printf("B2    %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->ef[1] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x06:                                                      // B3
                if (showTrace) printf("B3    %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->ef[2] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x07:                                                      // B4
                if (showTrace) printf("B4    %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->ef[3] != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x08:                                                      // NBR
                if (showTrace) trace("NBR\n");
                cpu->r[cpu->p]++;
                break;
           case 0x09:                                                      // BNQ
                if (showTrace) printf("BNQ   %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->q == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0a:                                                      // BNZ
                if (showTrace) printf("BNZ   %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->d != 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0b:                                                      // BNF
                if (showTrace) printf("BNF   %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->df == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0c:                                                      // BN1
                if (showTrace) printf("BN1   %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->ef[0] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0d:                                                      // BN2
                if (showTrace) printf("BN2   %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->ef[1] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0e:                                                      // BN3
                if (showTrace) printf("BN3   %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->ef[2] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           case 0x0f:                                                      // BN4
                if (showTrace) printf("BN4   %02X\n",cpu->ram[cpu->r[cpu->p]]);
                if (cpu->ef[3] == 0)
                  cpu->r[cpu->p] = (cpu->r[cpu->p] & 0xff00) |
                                   cpu->ram[cpu->r[cpu->p]];
                else
                  cpu->r[cpu->p]++;
                break;
           }
         break;
    case 4:                                                                // LDA
         cpu->d = cpu->ram[cpu->r[cpu->n]++];
         if (showTrace) {
           sprintf(tbuffer,"LDA   R%X             D=%02x\n",cpu->n,cpu->d);
           trace(tbuffer);
           }
         break;
    case 5:                                                                // STR
         if (showTrace) {
           sprintf(tbuffer,"STR   R%X             [%04x]=%02x\n",cpu->n,cpu->r[cpu->n], cpu->d);
           trace(tbuffer);
           }
         if (cpu->r[cpu->n] >= ramStart && cpu->r[cpu->n] <= ramEnd)
           cpu->ram[cpu->r[cpu->n]] = cpu->d;
         break;
    case 6:
         switch (cpu->n) {
           case 0x00:                                                      // IRX
                cpu->r[cpu->x]++;
                if (showTrace) {
                  sprintf(tbuffer,"IRX                  R%X=%04x\n",cpu->x,cpu->r[cpu->x]);
                  trace(tbuffer);
                  }
                break;
           }
         break;
    case 7:
         switch (cpu->n) {
           case 0x00:                                                      // RET
                if (showTrace) trace("RET\n");
                i = cpu->ram[cpu->r[cpu->x]++];
                cpu->p = i & 0xf;
                cpu->x = (i >> 4) & 0xf;
                cpu->ie = 1;
                break;
           case 0x01:                                                      // DIS
                if (showTrace) trace("DIS\n");
                i = cpu->ram[cpu->r[cpu->x]++];
                cpu->p = i & 0xf;
                cpu->x = (i >> 4) & 0xf;
                cpu->ie = 0;
                break;
           case 0x02:                                                      // LDXA
                cpu->d = cpu->ram[cpu->r[cpu->x]++];
                if (showTrace) {
                  sprintf(tbuffer,"LDXA                 D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x03:                                                      // STXD
                if (showTrace) {
                  sprintf(tbuffer,"STXD                 [%04x]=%02x\n",cpu->r[cpu->x], cpu->d);
                  trace(tbuffer);
                  }
                if (cpu->r[cpu->x] >= ramStart && cpu->r[cpu->x] <= ramEnd)
                  cpu->ram[cpu->r[cpu->x]] = cpu->d;
                cpu->r[cpu->x]--;
                break;
           case 0x04:                                                      // ADC
                w = cpu->d + cpu->ram[cpu->r[cpu->x]] + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"ADC                  D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x05:                                                      // SDB
                w = cpu->ram[cpu->r[cpu->x]] + ((~cpu->d) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"SDB                  D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x06:                                                      // SHRC
                if (showTrace) trace("SHRC\n");
                i = (cpu->d & 1);
                cpu->d = (cpu->d >> 1) | (cpu->df << 7);
                cpu->df = i;
                if (showTrace) {
                  sprintf(tbuffer,"SHRC                 D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x07:                                                      // SMB
                if (showTrace) trace("SMB\n");
                w = cpu->d + ((~cpu->ram[cpu->r[cpu->x]]) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"SMB                  D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x08:                                                      // SAV
                if (showTrace) trace("SAV\n");
                cpu->ram[cpu->r[cpu->x]] = cpu->t;
                break;
           case 0x09:                                                      // MARK
                if (showTrace) trace("MARK\n");
                cpu->t = (cpu->x << 4) | cpu->p;
                cpu->ram[cpu->r[2]--] = cpu->t;
                cpu->x = cpu->p;
                break;
           case 0x0a:                                                      // REQ
                if (showTrace) trace("REQ\n");
                cpu->q = 0;
                break;
           case 0x0b:                                                      // SEQ
                if (showTrace) trace("SEQ\n");
                cpu->q = 1;
                break;
           case 0x0c:                                                      // ADCI
                w = cpu->d + cpu->ram[cpu->r[cpu->p]++] + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"ADCI  %02x             D=%02x\n",cpu->ram[cpu->r[cpu->p]-1],cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x0d:                                                      // SDBI
                w = cpu->ram[cpu->r[cpu->p]++] + ((~cpu->d) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"SDBI  %02x             D=%02x\n",cpu->ram[cpu->r[cpu->p]-1],cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x0e:                                                      // SHLC
                if (showTrace) printf("SHLC\n");
                i = (cpu->d & 0x80) >> 7;
                cpu->d = (cpu->d << 1) | cpu->df;
                cpu->df = i;
                if (showTrace) {
                  sprintf(tbuffer,"SHLC                 D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x0f:                                                      // SMBI
                w = cpu->d + ((~cpu->ram[cpu->r[cpu->p]++]) & 0xff) + cpu->df;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"SMBI  %02x             D=%02x\n",cpu->ram[cpu->r[cpu->p]-1],cpu->d);
                  trace(tbuffer);
                  }
                break;
           }
         break;
    case 8:                                                                // GLO
         cpu->d = cpu->r[cpu->n] & 0xff;
         if (showTrace) {
           sprintf(tbuffer,"GLO   R%x             D=%02x\n",cpu->n,cpu->d);
           trace(tbuffer);
           }
         break;
    case 9:                                                                // GHI
         cpu->d = (cpu->r[cpu->n] >> 8) & 0xff;
         if (showTrace) {
           sprintf(tbuffer,"GHI   R%x             D=%02x\n",cpu->n,cpu->d);
           trace(tbuffer);
           }
         break;
    case 0x0a:                                                             // PLO
         cpu->r[cpu->n] = (cpu->r[cpu->n] & 0xff00) | cpu->d;
         if (showTrace) {
           sprintf(tbuffer,"PLO   R%X             R%X=%04x\n",cpu->n,cpu->n,cpu->r[cpu->n]);
           trace(tbuffer);
           }
         break;
    case 0x0b:                                                             // PHI
         cpu->r[cpu->n] = (cpu->r[cpu->n] & 0x00ff) | (cpu->d << 8);
         if (showTrace) {
           sprintf(tbuffer,"PHI   R%X             R%X=%04x\n",cpu->n,cpu->n,cpu->r[cpu->n]);
           trace(tbuffer);
           }
         break;
    case 0x0c:
         switch (cpu->n) {
           case 0x00:                                                      // LBR
                cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                  cpu->ram[cpu->r[cpu->p]+1];
                if (showTrace) {
                  sprintf(tbuffer,"LBR   %04x           *\n",cpu->r[cpu->p]);
                  trace(tbuffer);
                  }
                break;
           case 0x01:                                                      // LBQ
                if (showTrace) {
                  sprintf(tbuffer,"LBQ   %02X%02X           ",cpu->ram[cpu->r[cpu->p]],cpu->ram[cpu->r[cpu->p]+1]);
                  }
                if (cpu->q) {
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                  if (showTrace) strcat(tbuffer,"*\n");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tbuffer,"-\n");
                  }
                if (showTrace) trace(tbuffer);
                break;
           case 0x02:                                                      // LBZ
                if (showTrace) {
                  sprintf(tbuffer,"LBZ   %02X%02X           ",cpu->ram[cpu->r[cpu->p]],cpu->ram[cpu->r[cpu->p]+1]);
                  }
                if (cpu->d == 0) {
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                  if (showTrace) strcat(tbuffer,"*\n");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tbuffer,"-\n");
                  }
                if (showTrace) trace(tbuffer);
                break;
           case 0x03:                                                      // LBDF
                if (showTrace) {
                  sprintf(tbuffer,"LBDF  %02X%02X           ",cpu->ram[cpu->r[cpu->p]],cpu->ram[cpu->r[cpu->p]+1]);
                  }
                if (cpu->df != 0) {
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                  if (showTrace) strcat(tbuffer,"*\n");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tbuffer,"-\n");
                  }
                if (showTrace) trace(tbuffer);
                break;
           case 0x04:                                                      // NOP
                if (showTrace) trace("NOP");
                break;
           case 0x05:                                                      // LSNQ
                if (showTrace) printf("LSNQ  R%X\n",cpu->n);
                if (cpu->q == 0) cpu->r[cpu->p] += 2;
                break;
           case 0x06:                                                      // LSNZ
                if (showTrace) printf("LSNZ  R%X\n",cpu->n);
                if (cpu->d != 0) cpu->r[cpu->p] += 2;
                break;
           case 0x07:                                                      // LSNF
                if (showTrace) printf("LSNF  R%X\n",cpu->n);
                if (cpu->df == 0) cpu->r[cpu->p] += 2;
                break;
           case 0x08:                                                      // NLBR
                if (showTrace) printf("NLBR  R%X\n",cpu->n);
                cpu->r[cpu->p] += 2;
                break;
           case 0x09:                                                      // LBNQ
                if (showTrace) {
                  sprintf(tbuffer,"LBNQ  %02X%02X           ",cpu->ram[cpu->r[cpu->p]],cpu->ram[cpu->r[cpu->p]+1]);
                  }
                if (cpu->q == 0) {
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                  if (showTrace) strcat(tbuffer,"*\n");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tbuffer,"-\n");
                  }
                if (showTrace) trace(tbuffer);
                break;
           case 0x0a:                                                      // LBNZ
                if (showTrace) {
                  sprintf(tbuffer,"LBNZ  %02X%02X           ",cpu->ram[cpu->r[cpu->p]],cpu->ram[cpu->r[cpu->p]+1]);
                  }
                if (cpu->d != 0) {
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                  if (showTrace) strcat(tbuffer,"*\n");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tbuffer,"-\n");
                  }
                if (showTrace) trace(tbuffer);
                break;
           case 0x0b:                                                      // LBNF
                if (showTrace) {
                  sprintf(tbuffer,"LBNF  %02X%02X           ",cpu->ram[cpu->r[cpu->p]],cpu->ram[cpu->r[cpu->p]+1]);
                  }
                if (cpu->df == 0) {
                  cpu->r[cpu->p] = (cpu->ram[cpu->r[cpu->p]] << 8) |
                                    cpu->ram[cpu->r[cpu->p]+1];
                  if (showTrace) strcat(tbuffer,"*\n");
                  }
                else {
                  cpu->r[cpu->p] += 2;
                  if (showTrace) strcat(tbuffer,"-\n");
                  }
                if (showTrace) trace(tbuffer);
                break;
           case 0x0c:                                                      // LSIE
                if (showTrace) printf("LSIE  R%X\n",cpu->n);
                if (cpu->ie != 0) cpu->r[cpu->p] += 2;
                break;
           case 0x0d:                                                      // LSQ
                if (showTrace) printf("LSQ   R%X\n",cpu->n);
                if (cpu->q != 0) cpu->r[cpu->p] += 2;
                break;
           case 0x0e:                                                      // LSZ
                if (showTrace) printf("LSZ   R%X\n",cpu->n);
                if (cpu->d == 0) cpu->r[cpu->p] += 2;
                break;
           case 0x0f:                                                      // LSDF
                if (showTrace) printf("LSDF  R%X\n",cpu->n);
                if (cpu->df != 0) cpu->r[cpu->p] += 2;
                break;
           }
         break;
    case 0x0d:                                                             // SEP
         cpu->p = cpu->n;
         if (showTrace) {
           sprintf(tbuffer,"SEP   R%x             P=%x\n",cpu->n,cpu->p);
           trace(tbuffer);
           }
         break;
    case 0x0e:                                                             // SEX
         cpu->x = cpu->n;
         if (showTrace) {
           sprintf(tbuffer,"SEX   R%x             X=%x\n",cpu->n,cpu->x);
           trace(tbuffer);
           }
         break;
    case 0x0f:
         switch (cpu->n) {
           case 0x00:                                                      // LDX
                cpu->d = cpu->ram[cpu->r[cpu->x]];
                if (showTrace) {
                  sprintf(tbuffer,"LDX                  D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x01:                                                      // OR
                cpu->d |= cpu->ram[cpu->r[cpu->x]];
                if (showTrace) {
                  sprintf(tbuffer,"OR                   D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x02:                                                      // AND
                cpu->d &= cpu->ram[cpu->r[cpu->x]];
                if (showTrace) {
                  sprintf(tbuffer,"AND                  D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x03:                                                      // XOR
                cpu->d ^= cpu->ram[cpu->r[cpu->x]];
                if (showTrace) {
                  sprintf(tbuffer,"XOR                  D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x04:                                                      // ADD
                w = cpu->d + cpu->ram[cpu->r[cpu->x]];
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"ADD                  D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x05:                                                      // SD
                w = cpu->ram[cpu->r[cpu->x]] + ((~cpu->d) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"SD                   D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x06:                                                      // SHR
                i = (cpu->d & 1);
                cpu->d = (cpu->d >> 1);
                cpu->df = i;
                if (showTrace) {
                  sprintf(tbuffer,"SHR                  D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x07:                                                      // SM
                w = cpu->d + ((~cpu->ram[cpu->r[cpu->x]]) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"SM                   D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x08:                                                      // LDI
                cpu->d = cpu->ram[cpu->r[cpu->p]++];
                if (showTrace) {
                  sprintf(tbuffer,"LDI   %02x             D=%02x\n",cpu->d,cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x09:                                                      // ORI
                cpu->d |= cpu->ram[cpu->r[cpu->p]++];
                if (showTrace) {
                  sprintf(tbuffer,"ORI   %02x             D=%02x\n",cpu->ram[cpu->r[cpu->p]-1],cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x0a:                                                      // ANI
                cpu->d &= cpu->ram[cpu->r[cpu->p]++];
                if (showTrace) {
                  sprintf(tbuffer,"ANI   %02x             D=%02x\n",cpu->ram[cpu->r[cpu->p]-1],cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x0b:                                                      // XRI
                cpu->d ^= cpu->ram[cpu->r[cpu->p]++];
                if (showTrace) {
                  sprintf(tbuffer,"XRI   %02x             D=%02x\n",cpu->ram[cpu->r[cpu->p]-1],cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x0c:                                                      // ADI
                w = cpu->d + cpu->ram[cpu->r[cpu->p]++];
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"ADI   %02x             D=%02x\n",cpu->ram[cpu->r[cpu->p]-1],cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x0d:                                                      // SDI
                w = cpu->ram[cpu->r[cpu->p]++] + ((~cpu->d) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"SDI   %02x             D=%02x\n",cpu->ram[cpu->r[cpu->p]-1],cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x0e:                                                      // SHL
                i = (cpu->d & 0x80) >> 7;
                cpu->d = (cpu->d << 1);
                cpu->df = i;
                if (showTrace) {
                  sprintf(tbuffer,"SHL                  D=%02x\n",cpu->d);
                  trace(tbuffer);
                  }
                break;
           case 0x0f:                                                      // SMI
                w = cpu->d + ((~cpu->ram[cpu->r[cpu->p]++]) & 0xff) + 1;
                cpu->d = w & 0xff;
                cpu->df = (w > 255) ? 1 : 0;
                if (showTrace) {
                  sprintf(tbuffer,"SMI   %02x             D=%02x\n",cpu->ram[cpu->r[cpu->p]-1],cpu->d);
                  trace(tbuffer);
                  }
                break;
           }
         break;
    }
  }

