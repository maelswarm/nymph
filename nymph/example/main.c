#include "main.h"
#include <stdlib.h>
#include "box.h"

#include "rect.h"
static int add(int a, int b) {return a+b;
}
int main(int argc, const char * argv[]) {Box **myBoxes =malloc(sizeof(Box*));Box *myBox =malloc(sizeof(Box));myBox->height = 1;myBox->width = 1;myBox->depth = 1;myBoxes[0] =myBox;Rect *myRect =malloc(sizeof(Rect));myRect->height = 3;myRect->width = 3;int i =0;
    switch(add(1,0)) {case 1:
        printBox(myBoxes[0]);printRect(myRect);break;}
free(myBox);free(myBoxes);free(myRect);return 0;}
