#include "box.h"
#include <stdlib.h>
#include <stdio.h>
int add(int a, int b) {return a + b;
};
void printBox(Box *this) {printf("%i %i %i\n",1+add(2+this->height+2, this->height)+2,2+this->width+2, this->depth);
};


