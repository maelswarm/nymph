<p align="center"><img src="https://cdn.pixabay.com/photo/2013/07/12/12/13/fairy-145352_960_720.png" width="auto" height="200" /></p>

<h1 align=center>Nymph</h1>

<p align="center">Let's see what we can achieve by reworking C syntax.</p>

## Overview

Nymph is a simple C like programming language.

This repository contains a program compiled from file "nymph_pp.c". This program acts as a preprocessor, converting Nymph files (extension \*.n) into C files.

## Quick start

* [Download the repo](https://github.com/roecrew/nymph/archive/master.zip)

* Clone the repo: git clone ht&#8203;tps://git@github.com:roecrew/nymph.git

## Features

* [Object Oriented](https://en.wikipedia.org/wiki/Object-oriented_programming)

* [Function Overloading](https://en.wikipedia.org/wiki/Function_overloading)

## Goals

* Constructors and Destructors.

* Type Inference.

## Example
box.n

    #include <stdlib.h>
    #include <stdio.h>

    obj Box {
        int height = 1;
        int width = 1;
        int depth = 1;
    }

    int add(int a, int b) {
      return a + b;
    }

    pub void print(Box *this) {
        printf("%i %i %i\n",1+add(2+this->height+2, this->height)+2, 2+this->width+2, this->depth);
    }

rect.n

    #include <stdlib.h>
    #include <stdio.h>

    obj Rect {
        int height = 3;
        int width = 3;
    }

    pub void print(Rect *this) {
        printf("%i %i\n", this->height, this->width);
    }

main.n

    #include <stdlib.h>
    #include "box.n"
    #include "rect.n"

    int main(int argc, const char * argv[]) {

        Box **myBoxes = malloc(10*sizeof(Box*));
        Box *myBox = malloc(sizeof(Box));
        myBoxes[0] = myBox;
        Rect *myRect = malloc(sizeof(Rect));

        print(myBoxes[0]);
        print(myRect);

        free(myBox);
        free(myBoxes);
        free(myRect);

        return 0;
    }

makefile
```make
nymph: nymph_pp.c main.n box.n rect.n
	gcc -std=c11 -c helper_func.c helper_func.h
	gcc -std=c11 -c error_checking.c error_checking.h
	gcc -std=c11 -c nymph_pp.c
	gcc -std=c11 helper_func.o error_checking.o nymph_pp.o -o nymph
	./nymph main.n main
	gcc -std=c11 -c rect.c rect.h
	gcc -std=c11 -c box.c box.h
	gcc -std=c11 -c main.c main.h
	gcc -std=c11 main.o box.o rect.o -o out
```
