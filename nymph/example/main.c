#include "main.h"
#include <stdlib.h>
#include "box.h"

#include "rect.h"
static int add(int a, int b) {return a+b;
}
int main(int argc, const char * argv[]) {Box **myBoxes =malloc(sizeof(Box*));Box *myBox;Rect *myRect;myBox=malloc(sizeof(Box));myBox->height = 1;myBox->width = 1;myBox->depth = 1;myRect=malloc(sizeof(Rect));myRect->height = 3;myRect->width = 3;myBoxes[0]=myBox;volatile int *a =malloc(sizeof(int));printBox(myBoxes[0]);printRect(myRect);free(myBox);free(myBoxes);free(myRect);return 0;}
