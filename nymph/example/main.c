#include "main.h"
#include <stdlib.h>
#include "box.h"

#include "rect.h"
int main(int argc, const char * argv[]) {Box **myBoxes =malloc(sizeof(Box*));Box *myBox =malloc(sizeof(Box));myBox->height = 1;myBox->width = 1;myBox->depth = 1;myBoxes[0] =myBox;Rect *myRect =malloc(sizeof(Rect));myRect->height = 3;myRect->width = 3;printBox(myBoxes[0]);printRect(myRect);free(myBox);free(myRect);return 0;
};
