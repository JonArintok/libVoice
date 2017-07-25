
#include <stdio.h>

void sdlec(int line, const char *file);
#define _sdlec sdlec(__LINE__, __FILE__)

#define _SHOULD_NOT_BE_HERE_ printf("!! SHOULD NOT BE HERE: line %i of %s\n", __LINE__, __FILE__)
