#include "box.h"
#include <stdlib.h>
#include <stdio.h>
double cm_to_inchesdouble(double cm) {return cm / 2.54;
}


static int addintint(int a,int b) {return a + b;}

void printBoxBox(Box *this) {printf("%i\n",addintint(this->width,this->height));}
