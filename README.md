# nymph ![logo](http://icons.iconarchive.com/icons/iron-devil/ids-game-world/32/Fairy-icon.png)
<br/>
In Nymph, String means char *.
<br/>
<br/>
In Nymph, ADTs and primitive data type functions are simplified.
<br/>
<br/>

    //box.n
    
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

    //main.n
    
    #include <stdlib.h>
    #include "box.n"

    int main(int argc, const char * argv[]) {

        Box *myBox;
        &myBox.create();
        myBox.print();
        free(myBox);

        return 0
    }
