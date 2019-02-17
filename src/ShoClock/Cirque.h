struct Cirque{
  unsigned int head; // First Element
  unsigned int tail; // 1 past the last element
  unsigned int isFull;
  double * entries; 
  unsigned int size;
};

typedef struct Cirque Cirque;

// typedef -> https://overiq.com/c-programming-101/typedef-statement-in-c/
// Syntax: typedef data_type new_name;
//typedef void (*cirqueForFn) (double *);

Cirque * cirqueCreate();

void cirqueDelete(Cirque * queue);

unsigned int cirqueInsert(Cirque * queue, double data);

double cirqueRemove(Cirque * queue);

double cirquePeek(const Cirque * queue);

unsigned int cirqueGetCount(const Cirque * queue);

//void cirqueForEach(const Cirque * queue, cirqueForFn fun);
