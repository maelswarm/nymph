#include "box.h"
#include <stdlib.h>
#include <stdio.h>
int addintint(int a, int b);

 void printBox1(Box *this){printf("%i\n",addintint(this->width,this->height));}

int addintint(int a, int b){  return a + b;}