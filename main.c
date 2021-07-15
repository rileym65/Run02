#define MAIN

#include "header.h"

int main(int argc, char** argv) {
  struct termios terminal;
  if (argc != 2) {
    printf("Usage: run02 filename\n");
    exit(1);
    }
  loader(argv[1]);
  
  tcgetattr(0,&terminal);
  original = terminal;
  terminal.c_lflag &= ~ICANON;
  terminal.c_lflag &= ~ECHO;
  if (tcsetattr(0,TCSANOW,&terminal) != 0) {
    printf("Could not set terminal attributes\n");
    exit(1);
    }
  runFlag = 0xff;
  cpuReset(&cpu);
  while (runFlag) {
    cpuCycle(&cpu);
    if (cpu.idle) runFlag = 0;
    }
  if (tcsetattr(0,TCSANOW,&original) != 0) {
    printf("Could not restore terminal attributes\n");
    }
  }

