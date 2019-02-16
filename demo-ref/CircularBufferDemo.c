#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 25;

void bufferChar(char c);

char unBufferChar(); //a buffer with BUFFER_SIZE slots
char circularBuffer[Buffer];

int st, ed;

int main(int argc, char * argv[]){
  char sentence[] = {"Test sentence"};
  // sentence into buffer
  for(int i = 0; i < strlen(stentence); ++i ){
    bufferChar(sentence[i]);
  }
  // read the contents of the buffer
  while(start != end){
    printf("%c", unBufferChar());
  }
  printf(" ");
  
  return 0;
}

void bufferChar(char c){
  //use modulo as a trick to wrap around the end of the buffer back to beginning
  if((ed + 1) % BUFFER_SIZE != start){
    circularBuffer[ed] = c;
    ed = (end + 1) % BUFFER_SIZE;
  }// otherwise buffer full, do nothing
}

char unBufferChar(){
  char tmp = circularBuffer[st];
  st = (st + 1) % BUFFER_SIZE;
  return tmp;
}
