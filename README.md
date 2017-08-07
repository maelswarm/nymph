# nymph ![logo](http://icons.iconarchive.com/icons/iron-devil/ids-game-world/32/Fairy-icon.png)
<br/>
In Nymph, String means char *.
<br/>
<br/>
In Nymph, ADTs and primitive data type functions are simplified.
<br/>
<br/>
In Nymph, terminators are not semicolons, but newlines.
<br/>
<br/>

    #include <stdio.h>

    void String.world(int b,int c) { //object function declaration
        printf("%c%c\n", this[b], this[c]) //"this" is the string object
    }

    int main(int argc, const char * argv[]) {

        String hello = "Blah"
        int blade = 1
        int runner = 2

        hello.world(blade,runner) //calling object function

        return 0
    }
