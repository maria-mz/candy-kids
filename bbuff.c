#include <stdbool.h>
#include <stdio.h>

#define BUFFER_SIZE 10
void *buffer[BUFFER_SIZE];
int count;

void bbuff_init(void) {
  count = 0;
}

void bbuff_blocking_insert(void* item) {
  buffer[count] = item;
  count++;
}

void *bbuff_blocking_extract(void) {
  void *item = buffer[count - 1];
  count--;
  return item;
}

_Bool bbuff_is_empty(void) {
  return count == 0;
}