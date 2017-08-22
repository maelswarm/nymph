#include "main.h"
#include <stdlib.h>
#include "box.h"
#include "rect.h"

char returnH(){return 'H';}

int subtract(int a, int b){return a- b;}

int main(int argc, const char *argv[]){Box**myBoxes= malloc(10*sizeof(Box *));Box*myBox;Rect*myRect;myBox= malloc(sizeof(Box));myBox->width=1;myBox->height=1;myBox->depth=1;myRect= malloc(sizeof(Rect));myRect->height=1;myRect->width=1;myBoxes[0]= myBox;int b= subtract(2, 1);int d= (b==subtract(subtract(subtract(2,1),subtract(2,1)),subtract(2, 1)));for(int i=subtract(2, 1);i<subtract(2, 1);i++) {if((d= subtract(2, 1))){}}printBox(myBoxes[0]);printRect(myRect);free(myBox);free(myBoxes);free(myRect);return 0;}