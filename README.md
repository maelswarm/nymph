# nymph ![logo](http://icons.iconarchive.com/icons/iron-devil/ids-game-world/32/Fairy-icon.png)
Let's see what we can achieve by reworking C syntax.

## Updates
A new parser has been implemented.

## Goals
Polish default object member values.

## Example
box.n

    #include <stdlib.h>
    #include <stdio.h>

    object Box {
        int height = 3;
        int width = 5;
        int depth = 9;
    }

    public void Box*.print() {
        printf("%i %i %i\n", this->height, this->width, this->depth);
    }

    public void Box**.create() {
        (*this) = new Box;
        (*this)->width = 1;
        (*this)->height = 1;
        (*this)->depth = 1;
    }

rect.n

    #include <stdlib.h>
    #include <stdio.h>

    object Rect {
        int height = 1;
        int width = 1;
    }

    public void Rect*.print() {
        printf("%i %i\n", this->height, this->width);
    }

    public void Rect**.create() {
        (*this) = new Rect;
    }

main.n

    #include <stdlib.h>
    #include "box.n"
    #include "rect.n"

    int main(int argc, const char * argv[]) {

        Box *myBox;
        Rect *myRect;
        
        &myBox.create();
        &myRect.create();
        
        myBox.print();
        myRect.print();
        
        free(myBox);
        free(myRect);

        return 0;
    }

makefile

    nymph: nymph_compiler.c
            gcc -std=c11 nymph_compiler.c -o nymph
            ./nymph rect.n rect
            ./nymph box.n box
            ./nymph main.n main
            gcc -std=c11 -c box.c box.h
            gcc -std=c11 -c rect.c rect.h
            gcc -std=c11 -c main.c main.h
            gcc -std=c11 main.o box.o rect.o -o out
