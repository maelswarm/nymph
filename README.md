# nymph
A slightly different version of C.

Example program:

File: test_nymph.n

    #include <stdio.h>; //semi-colons are needed after include directives

    int int.myWorld(String a,float b) { //primitive data type prototype function declaration
        printf("%s %f", a, b);
        return this;
    }

    void String.world(int b,int c) { //object protype function declaration
        printf("%c%c\n", this[b], this[c]); //"this" is the string object
    }

    int main(int argc, const char * argv[]) {

        String hello = "Blah";
        int blade = 1;
        int runner = 2;

        blade.myWorld(hello,(float)runner); //calling primitive data type prototype function

        hello.world(blade,runner); //calling object prototype function

        return 0;
    }
