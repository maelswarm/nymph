#include "main.h"
#include <stdlib.h>
#include "box.h"
#include "rect.h"

int main(int argc, const char * argv[]) {Box *myBox;Rect *myRect;createBox(&myBox);createRect(&myRect);printBox(myBox);printRect(myRect);free(myBox);free(myRect);return 0;};
