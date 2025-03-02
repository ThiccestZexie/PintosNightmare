#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "memory.h"

/* Recommended compile command:
 *
 * make debug1
 *
 * Run your code:
 *
 * ./debug1
 *
 * Debug with GDB:
 *
 * gdb -tui ./debug1
 */


int main(void)
{
  int values = 5;

  // This is a normal array of integers
  int *contents = malloc(sizeof(int) * values);

  // This is an array of pointers to integers
  // Hint: int *data[];
  int **data = malloc(sizeof(int *) * values);

  // Fill with data:
  for (int i = 0; i < values; i++) {
    contents[i] = i * 5;
    data[i] = &contents[i];
  }

  // Print data:
  for (int i = 0; i <= values; i++) {
    int *pointer = data[i];
    int value = *pointer;
    printf("At %d: %d\n", i, value);
  }

  free(data);
  free(contents);

  return 0;
}
