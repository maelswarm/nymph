#include "main.h"
#include <stdlib.h>
#include "box.h"
#include "rect.h"

char returnH ŒöZ÷(){  return 'H';}

int subtractintint(int a, int b){  return a - b;}

int main(int argc, const char *argv[]){    Box **myBoxes = malloc(10*sizeof(Box *));;Box *myBox;Rect *myRect;    myBox = malloc(sizeof(Box));myBox->width=1;myBox->height=1;myBox->depth=1;;    myRect = malloc(sizeof(Rect));myRect->height=1;myRect->width=1;;    myBoxes[0] = myBox;int q =subtractintint(1,2)+subtractintint(2,1);int b =subtractintint((int)2,1+(int)subtractintint(2,1));    int d = (b ==subtractintint(1,2)+subtractintint(2,1));for(int i =subtractintint(2,1);i<subtractintint(2,1);i+=subtractintint(2,1)){if(subtractintint(2,1)){int z;}}for(int i = 0; i<10; i++) {if(subtractintint(2,1)){int z;}}printBox1(myBoxes[0]);printRect1(myRect);free(myBox);free(myBoxes);free(myRect);    return 0;}