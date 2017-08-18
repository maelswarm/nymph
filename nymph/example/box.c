#include "box.h"
#include <stdlib.h>
#include <stdio.h>
static int add(int a, int b) {return a + b;}
void printBox(Box *this) {printf("%i\n",add(this->width, this->height));}
