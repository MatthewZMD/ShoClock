struct Cirque{
  unsigned int head; // First Element
  unsigned int tail; // 1 past the last element
  unsigned int isFull;
  void ** entries; // void ** -> https://stackoverflow.com/a/9040946/11030684
  unsigned int size;
};

typedef struct Cirque Cirque;

// typedef -> https://overiq.com/c-programming-101/typedef-statement-in-c/
// Syntax: typedef data_type new_name;
typedef void (*cirqueForFn) (void*);

Cirque * cirqueCreate();

void cirqueDelete(Cirque * queue);

unsigned int cirqueInsert(Cirque * queue, void * data);

void * ciqueRemove(Cirque * queue);

void * cirquePeek(const Cirque * queue);

unsigned int cirqueGetCount(const Cirque * queue);

void cirqueForEach(const Cirque * queue, cirqueForFn)
