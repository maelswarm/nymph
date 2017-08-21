#include "main.h"
#include <stdlib.h>
#include "box.h"

#include "rect.h"

static int addlongintint(long a, int hi, int b);
static char returnH() {return 'H';
}

static int subtractintint(int a, int b) {return a - b;}

int main(int argc, const char * argv[]) {Box **myBoxes =malloc(sizeof(Box*));Box *myBox;Rect *myRect;myBox =malloc(sizeof(Box));myBox->width = 1;myBox->height = 1;myBox->depth = 1;myRect =malloc(sizeof(Rect));myBoxes[0] =myBox;volatile long *a =malloc(sizeof(long));int b =subtractintint(2,1);int d;
    if((d =subtractintint(2,1))) {int c =addlongintint(*a, (int)returnH(),5);}
printBox1(myBoxes[0]);printRect1(myRect);free(myBox);free(myBoxes);free(myRect);return 0;}

static int addlongintint(long a, int hi, int b) {return (int)a+b;}
