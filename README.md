<p align="center"><img src="https://cdn.pixabay.com/photo/2013/07/12/12/13/fairy-145352_960_720.png" width="auto" height="200" /></p>

<h1 align=center>Nymph</h1>

<p align="center">Let's see what we can achieve by reworking C syntax.</p>

## Overview

Nymph is a simple C like programming language.

Nymph acts as a preprocessor, converting Nymph files (extension \*.n) into C files.

This project is very much in development... It is not production ready.

## What's New

This project just recieved a complete overhaul.

More documentation will be added this week.

## Goals

### Completed

* Class-Based OOP

* Subtyping

### In Progress

* TBD

### Pending

* Destructors?

* Type Inference?

* Reflection?

* Default function arguments?

* Lambdas?

## Example

mammal.n
```
#include <stdio.h>
#include <stdlib.h>

class Mammal {

    + int population = 0;             // Class Variable (+)
    - int height = 0, weight = 100;   // Object Variable (-)

    + Mammal *init(int height, int weight) {  // Class Method (+) Constructor
        this->height = height;
        this->weight = weight;
        Mammal->population++;
        return this;
    }

    - void print() {                          // Object Method (-)
        printf("print instance properties...\n");
    }
}
```

human.n
```
#include "mammal.n"
#include <stdio.h>
#include <stdlib.h>

class Human : Mammal {

    - char *name = NULL; // Object Variable (-)

    + Human *init(char *name, int height, int weight) { // Class Method (+) Constructor
        this = super->init(height, weight);
        this->name = name;
        return this;
    }

    - void died() {                                     // Object Method (-) Constructor
        free(this->name);
        free(this);
        Mammal->population--;
    }
}

int main(void) {

    char *name = malloc(5);
    memset(name, 0, sizeof(name));
    strcpy(name, "Fred");
    Human *person1 = Human->init(name, 76, 146); // Class Method Constructor Call
    person1->print();                            // Object Method Call
    person1->died();                             // Object Method Call

    return 0;
}
```

```
nymph -r human.n
```