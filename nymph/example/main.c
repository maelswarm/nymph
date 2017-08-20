#include "main.h"
#include <stdlib.h>
#include "box.h"

#include "rect.h"

static 

 int addintchar1int(int a, char *hi, int b) {return a+b;
}

static char * returnHello() {return "Hello";}

int  main(int argc, const char * argv[]) {Box **myBoxes =malloc(sizeof(Box*));Box *myBox;Rect *myRect;myBox =malloc(sizeof(Box));myRect =malloc(sizeof(Rect));myRect->height = 3;myRect->width = 3;myBoxes[0] =myBox;volatile unsigned int *a =malloc(sizeof(int));int c =addintchar1int(*a,returnHello(),5);printBox1(myBoxes[0]);printRect1(myRect);free(myBox);free(myBoxes);free(myRect);return 0;}
