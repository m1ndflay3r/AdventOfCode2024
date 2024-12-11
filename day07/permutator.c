#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Generate permutations w/ recursion
void generatePermutations(char *str, int index, int length) {
  if (index == length) {
    printf("%s\n", str);
    return;
  }
  // Assign '*' to current index & recurse
  str[index] = '*';
  generatePermutations(str, index + 1, length);
  // Assign '+' to current index & recurse
  str[index] = '+';
  generatePermutations(str, index + 1, length);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <length>\n", argv[0]);
    return 1;
  }
  int length = atoi(argv[1]);
  if (length <= 0) {
    fprintf(stderr, "Error: Length must be a positive integer.\n");
    return 1;
  }
  // Allocate mem for string, set null term.
  char *str = (char *)malloc((length + 1) * sizeof(char));
  if (str == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    return 1;
  }
  str[length] = '\0'; // Null-term
  // Generate + print permutations
  generatePermutations(str, 0, length);
  // Free mem
  free(str);
  return 0;
}
