#include "rect.h"
#include <stdlib.h>
#include <stdio.h>

struct Rect {
    int height;
    int width;
};
void printRect1(Rect* rebrab) {printf("%i %i\n", rebrab->height, rebrab->width);};
void createRect11(Rect** rebrab) {(*rebrab)=malloc(sizeof(Rect));(*rebrab)->width =1;(*rebrab)->height =1;};
