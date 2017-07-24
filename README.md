# nymph
A slightly different version of C.
<br/>
<br/>
In Nymph, String means char \*.

In Nymph, ADTs and primitive data types functions are simplified.
<br/>
<br/>
Simply compile your nymph \*.c file with ./nymph \<file to compile into c\> \<output name\>

Example program:

File: test_nymph.c

    #include <stdio.h>

    void String.world(int b,int c) { //object function declaration
        printf("%c%c\n", this[b], this[c]); //"this" is the string object
    }

    int main(int argc, const char * argv[]) {

        String hello = "Blah";
        int blade = 1;
        int runner = 2;

        hello.world(blade,runner); //calling object function

        return 0;
    }
