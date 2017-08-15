#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include "box.h"

#include "rect.h"
char* printchar(char* rebrab,char *a) {printf("%s %s\n", rebrab, a);return rebrab;};
int main(int argc, const char * argv[]) {char *a ="hi";char *b ="hello";printchar(a,printchar(b,a));Box *myBox;Rect *myRect;createBox(&myBox);createRect(&myRect);printBox(myBox);printRect(myRect);free(myBox);free(myRect);return 0;};
