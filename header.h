#ifndef _HEADER_H
#define _HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

typedef u_int16_t word;
typedef u_int8_t byte;

#ifdef MAIN
#define LINK
#else
#define LINK extern
#endif

typedef struct {
  byte p;
  byte x;
  word r[16];
  byte d;
  byte df;
  byte i;
  byte n;
  byte ie;
  byte q;
  byte t;
  byte ef[4];
  byte idle;
  byte ram[65536];
  } CPU;

LINK CPU  cpu;
LINK byte runFlag;
LINK byte useElfos;
LINK byte trace;
LINK struct termios original;

extern int loader(char* filename);

#endif


