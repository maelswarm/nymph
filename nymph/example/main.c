#include "main.h"
#include <stdlib.h>
#include "box.h"

#include "rect.h"

static int addintint(int a,int b) {return a+b;
}

int main(int argc,const char * argv[]) {Box **myBoxes =malloc(sizeof(Box*));Box *myBox;Rect *myRect;myBox =malloc(sizeof(Box));myRect =malloc(sizeof(Rect));myRect->height = 3;myRect->width = 3;myBoxes[0] =myBox;volatile unsigned int *a =malloc(sizeof(int));int c =addintint(*a,5);printBoxBox(myBoxes[0]);printRectRect(myRect);free(myBox);free(myBoxes);free(myRect);return 0;}
