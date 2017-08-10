# nymph ![logo](http://icons.iconarchive.com/icons/iron-devil/ids-game-world/32/Fairy-icon.png)
<br/>
In Nymph, String means char *.
<br/>
<br/>
In Nymph, ADTs and primitive data type functions are simplified.
<br/>
<br/>


    #include <stdio.h>
    #include <stdlib.h>
    #include "nymph.n" //be sure to include the current filename

    object Box {
      int height;
      int width;
      int depth;
    }

    public void Box *.print() {
        printf("%i %i %i\n", this->height, this->width, this->depth);
    }

    public void Box *.create(int w, int h, int d) {
        this = malloc(sizeof(Box));
        this->width = w;
        this->height = h;
        this->depth = d;
    }

    int main(int argc, const char * argv[]) {

        Box *myBox;
        myBox.create();
        myBox.print();

        return 0
    }
