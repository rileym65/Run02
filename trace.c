#include "header.h"

void trace(char* message) {
  if (useVisual) output(message);
    else printf("%s\n",message);
  }

