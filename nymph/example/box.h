double cm_to_inches(double cm);
typedef struct Box Box;

struct Box{ 
  int width ;
  int height ;
  int depth ;
  double (*func1)(double) ;
};
void printBox(Box *this);
