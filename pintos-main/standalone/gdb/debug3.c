#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Recommended compile command:
 *
 * make debug3
 *
 * Run your code:
 *
 * ./debug3
 *
 * Debug with GDB:
 *
 * gdb -tui ./debug3
 */


// Create a copy of a string. Basically, this should work just like "strdup" in
// the standard library (see "man strdup" for details).
char *my_strdup(const char *original)
{
  int length = strlen(original);
  char *copy = malloc(sizeof(char) * length);
  for (int i = 0; i < length; i++)
    copy[i] = original[i];
  return copy;
}


int main(void)
{
  const char *original = "Strings in C are fun!";
  char *copy = my_strdup(original);

  printf("Original: %s\n", original);
  printf("Copy:     %s\n", copy);

  free(copy);

  return 0;
}
