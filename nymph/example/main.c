#include "main.h"
#include <stdlib.h>
#include "box.h"
#include "rect.h"
int main(int argc, const char * argv[]) {
Box *myBox;
Rect *myRect;
;
create(&myBox);
;
create(&myRect);
;
;
print(myBox);
;
print(myRect);
;
;
free(myBox);
free(myRect);
return 0;
};
