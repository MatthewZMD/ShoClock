#include <stdlib.h>
#include "Cirque.h"

// create cirque
Cirque * cirqueCreate(){
  const unsigned int size = 4;
  Cirque * queue = malloc(sizeof(Cirque));
  if(queue){
    queue->entries = malloc(size * sizeof(void *));
    if(queue->entries){
      queue->size = size;
      queue->head = 0;
      queue->tail = 0;
      queue->isFull = 0;
    }else{
      free(queue);
      queue = NULL;
    }
  }
  return queue;
}

// delete cirque
void cirqueDelete(Cirque * queue){
  if(queue){
    free(queue->entries);
    free(queue);
  }
}

// resize cirque
static void CirqueResize(Cirque * queue){
  void ** tmp = malloc(queue->size * 2 * sizeof(void *));
  if(tmp){
    unsigned int i = 0;
    ungisned int h = queue->head;
    do{
      tmp[i] = queue->entries[h++];
      if(h == queue->size){
        h = 0;
      }
      ++i;
    }while(h != queue->tail);
    free(queue->entries);
    queue->entries = tmp;
    queue->head = 0;
    queue->tail = queue->size;
    queue->size *= 2;
    queue->isFull = 0;
  }
}

// check whether cirque is empty
static unsigned int cirqueIsEmpty(const Cirque * queue){
  return (queue->head == queue->tail) && !queue->isFull;
}

// insert data into cirque
unsigned int cirqueInsert(Cirque * queue, void * data){
  unsigned int result;
  if(queue->isFull){
    cirqueResize(queue);
    if(queue->isFull){
      result = 0;
    }
  }else{
    queue->entries[queue->tail++] = data;
    if(queue->tail == queue->size){
      queue->tail = 0;
    }
    if(queue->tail == queue->head){
      queue->isFull = 1;
    }
    result = 1;
  }
  return result;
}

// remove cirque
void * ciqueRemove(Cirque * queue){
  void * data = NULL;
  if(!cirqueIsEmpty(queue)){
    if(queue->isFull){
      queue->isFull = 0;
    }
    data = queue->entries[queue->head++];
    if(queue->head == queue->size){
      queue->head = 0;
    }
  }
  return data;
}

// return head cirque
void * cirquePeek(const Cirque * queue){
  void * data = NULL;
  if(!cirqueIsEmpty(queue)){
    data = queue->entries[queue->head];
  }
  return data;
}

// count number of data
unsigned int cirqueGetCount(const Cirque * queue){
  unsigned int count;
  if(cirqueIsEmpty(queue)){
    count = 0;
  }else if(queue->isFull){
    count = queue->size;
  }else if(queue->tail > queue->head){
    count = queue->tail - queue->head;
  }else{
    count = queue->size - queue->head;
    if(queue->tail > 0){
      count += queue->tail -1;
    }
  }
  return count;
}

void cirqueForEach(const Cirque * queue, cirqueForFn fun){
  if(!cirqueIsEmpty(queue)){
    unsigned int h = queue->head;
    do{
      fun(queue->entries[h++]);
      if(h == queue->size){
        h = 0;
      }
    }while(h != queue->tail);
  }
}
