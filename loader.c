#include "header.h"

byte convHex(char* buffer) {
  byte ret;
  ret = 0;
  if (*buffer >= '0' && *buffer <= '9') ret = *buffer - '0';
  if (*buffer >= 'a' && *buffer <= 'f') ret = *buffer - 'a' + 10;
  if (*buffer >= 'A' && *buffer <= 'F') ret = *buffer - 'A' + 10;
  ret <<= 4;
  buffer++;
  if (*buffer >= '0' && *buffer <= '9') ret |= *buffer - '0';
  if (*buffer >= 'a' && *buffer <= 'f') ret |= *buffer - 'a' + 10;
  if (*buffer >= 'A' && *buffer <= 'F') ret |= *buffer - 'A' + 10;
  return ret;
  }

int loader(char* filename) {
  int   i;
  FILE* infile;
  char  buffer[1024];
  word  address;
  byte  len;
  byte  typ;
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
    if (buffer[0] == ':' && buffer[5] != ' ') {
      pos = 1;
      len = convHex(buffer+pos);
      pos += 2;
      if (len > 0) {
        address = convHex(buffer+pos) << 8;
        pos += 2;
        address |= convHex(buffer+pos);
        pos += 2;
        typ = convHex(buffer+pos);
        pos += 2;
        if (typ == 0) {
          for (i=0; i<len; i++) {
            value = convHex(buffer+pos);
            pos += 2;
            cpu.ram[address++] = value;
            }
          }
        }
      }
    else {
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
      }
    if (valid) cpu.ram[address++] = value;
    }
  return 0;
  }

