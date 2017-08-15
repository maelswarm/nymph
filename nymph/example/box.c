#include "box.h"
#include <stdlib.h>
#include <stdio.h>

struct Box {
    int height;
    int width;
    int depth;
};
void printBox1(Box* rebrab) {printf("%i %i %i\n", rebrab->height, rebrab->width, rebrab->depth);};
void createBox11(Box** rebrab) {(*rebrab)=malloc(sizeof(Box));(*rebrab)->width =1;(*rebrab)->height =1;(*rebrab)->depth =1;};
