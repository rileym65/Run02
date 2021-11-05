#include "header.h"

int loader(char* filename) {
  FILE* infile;
  char  buffer[1024];
  word  address;
  byte  value;
  byte  valid;
  char  mode;
  int   pos;
  infile = fopen(filename,"r");
  if (infile == NULL) {
    printf("Could not open input file: %s\n",filename);
    exit(1);
    }
  address = 0;
  value = 0;
  valid = 0;
  mode = 'B';
  while (fgets(buffer, 1023, infile) != NULL) {
    pos = 0;
    while (buffer[pos] != 0) {
      if (buffer[pos] == ':') {
        if (valid)
          cpu.ram[address++] = value;
        valid = 0;
        mode = 'A';
        address = 0;
        }
      else if (buffer[pos] == ' ') {
        if (valid) 
          if (mode == 'B') cpu.ram[address++] = value;
        valid = 0;
        mode = 'B';
        value = 0;
        }
      else if (buffer[pos] >= '0' && buffer[pos] <= '9') {
        if (mode == 'A')
          address = (address << 4) | (buffer[pos] - '0');
        else
          value = (value << 4) | (buffer[pos] - '0');
        valid = 0xff;
        }
      else if (buffer[pos] >= 'a' && buffer[pos] <= 'f') {
        if (mode == 'A')
          address = (address << 4) | (10 + buffer[pos] - 'a');
        else
          value = (value << 4) | (10 + buffer[pos] - 'a');
        valid = 0xff;
        }
      else if (buffer[pos] >= 'A' && buffer[pos] <= 'F') {
        if (mode == 'A')
          address = (address << 4) | (10 + buffer[pos] - 'A');
        else
          value = (value << 4) | (10 + buffer[pos] - 'A');
        valid = 0xff;
        }
      pos++;
      }
    if (valid) cpu.ram[address++] = value;
    }
  return 0;
  }

