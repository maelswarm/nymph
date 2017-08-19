double cm_to_inchesdouble(double cm);
typedef struct Box Box;

struct Box{ int width;int height;int depth;double (*func1)(double) ;
};
void printBoxBox(Box *this);
