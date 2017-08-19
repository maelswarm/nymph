typedef struct Box Box;

struct Box{ 
    volatile int height ;
    int width ;
    int depth ;
};
void printBox(Box *this);
