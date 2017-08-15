#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include "box.h"

#include "rect.h"


int main(int argc, const char * argv[]) {Box *myBox;Rect *myRect;createBox11(&myBox);createRect11(&myRect);printBox1(myBox);printRect1(myRect);free(myBox);free(myRect);return 0;};
