#include "box.h"
#include <stdlib.h>
#include <stdio.h>
double cm_to_inches(double cm) {return cm / 2.54;
}


static int add(int a, int b) {return a + b;}

void printBox(Box *this) {printf("%i\n",add(this->width, this->height));}
