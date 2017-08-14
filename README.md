# nymph ![logo](http://icons.iconarchive.com/icons/iron-devil/ids-game-world/32/Fairy-icon.png)
<br/>
In Nymph, String means char *.
<br/>
<br/>
In Nymph, ADTs and primitive data type functions are simplified.
<br/>
<br/>

box.n

    #include <stdlib.h>
    #include <stdio.h>

    object Box {
        int height;
        int width;
        int depth;
    }

    public void Box*.print() {
        printf("%i %i %i\n", this->height, this->width, this->depth);
    }

    public void Box**.create() {
        (*this) = malloc(sizeof(Box));
        (*this)->width = 1;
        (*this)->height = 1;
        (*this)->depth = 1;
    }

rect.n

    #include <stdlib.h>
    #include <stdio.h>

    object Rect {
        int height;
        int width;
    }

    public void Rect*.print() {
        printf("%i %i\n", this->height, this->width);
    }

    public void Rect**.create() {
        (*this) = malloc(sizeof(Rect));
        (*this)->width = 1;
        (*this)->height = 1;
    }

main.n

    #include <stdlib.h>
    #include "box.n"
    #include "square.n"

    int main(int argc, const char * argv[]) {

        Box *myBox;
        Rect *myRect;
        
        &myBox.create();
        &myRect.create();
        
        myBox.print();
        myRect.print();
        
        free(myBox);
        free(myRect);

        return 0
    }

makefile

    nymph: nymph_compiler.c
            gcc -std=c11 nymph_compiler.c -o nymph
            ./nymph square.n square
            ./nymph box.n box
            ./nymph main.n main
            gcc -std=c11 -c square.c square.h
            gcc -std=c11 -c box.c box.h
            gcc -std=c11 -c main.c main.h
            gcc -std=c11 main.o box.o nymph.o -o out
