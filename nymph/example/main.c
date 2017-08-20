#include "main.h"
#include <stdlib.h>
#include "box.h"

#include "rect.h"

static 

 int addlongintint(unsigned long a, int hi, int b) {return (int)a+b;
}

static char  returnH() {return 'H';}

int  main(int argc, const char * argv[]) {Box **myBoxes =malloc(sizeof(Box*));Box *myBox;Rect *myRect;myBox =malloc(sizeof(Box));myBox->width = 1;myBox->height = 1;myBox->depth = 1;myRect =malloc(sizeof(Rect));myBoxes[0] =myBox;volatile unsigned long *a =malloc(sizeof(long));int c =addlongintint(*a, (int)returnH(),5);printBox1(myBoxes[0]);printRect1(myRect);free(myBox);free(myBoxes);free(myRect);return 0;}
