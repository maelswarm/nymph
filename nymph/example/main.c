#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include "box.h"

#include "rect.h"
int main(int argc, const char * argv[]) {Box *myBox =malloc(sizeof(Box));myBox->height = 1;myBox->width = 1;myBox->depth = 1;Rect *myRect =malloc(sizeof(Rect));myRect->height = 3;myRect->width = 3;printBox(myBox);printRect(myRect);free(myBox);free(myRect);return 0;
};
