#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
#include "Cirque.h"
 
int main(void)
{
    Cirque * queue;
    char buf[16];
    unsigned int f;
    const unsigned int max = 32;
    const unsigned int limit = 16;
 
    queue = cirqueCreate();
    for (f = 0; f < max; f++) {
      //sprintf(buf, "Max %d, Item %d", max, f);
      sprintf(buf, "Item %d", f);
      if (f >= limit) {
        /* Start removing at limit to show the queue doesn't keep growing */
        char *data = cirqueRemove(queue);
        printf("%s is Removed\n", data);
        free(data);
      }
      printf("Inserting %s\n", buf);
      cirqueInsert(queue, strdup(buf));
    }
    printf("Displaying remaining data now...\n");
    cirqueForEach(queue, (cirqueForFn)puts);
    printf("Size is %d (should be %d)\n", queue->size, limit);
    cirqueForEach(queue, free);
    cirqueDelete(queue);
 
    return 0;
}
