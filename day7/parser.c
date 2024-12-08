#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OUT 1024

// Function to convert input string to zsh format, ensure left-right math
// why do it this way you ask?
// because I can of course!
void convertToZshMath(const char *input, char *output) {
  int length = strlen(input);
  char temp[MAX_OUT] = {0};
  int pos = 0;
  // first number
  int i = 0;
  while (i < length && (input[i] >= '0' && input[i] <= '9')) {
    temp[pos++] = input[i++];
  }
  // Process rest of string
  while (i < length) {
    // Wrap current expr. in $(( )) w/ next operator & num
    char current[MAX_OUT] = {0};
    snprintf(current, MAX_OUT, "$((%s%c", temp, input[i++]);
    // Add next num
    while (i < length && (input[i] >= '0' && input[i] <= '9')) {
      size_t len = strlen(current);
      current[len] = input[i++];
      current[len + 1] = '\0';
    }
    // Closing parenthesis
    strcat(current, "))");
    // Copy new expr back to temp
    strcpy(temp, current);
  }
  // Copy result to output buffer
  strcpy(output, temp);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <equation>\n", argv[0]);
    return 1;
  }
  const char *input = argv[1];
  char output[MAX_OUT];
  // Ensure input contains only num, '+' or '*'
  for (int i = 0; input[i] != '\0'; i++) {
    if (!(input[i] >= '0' && input[i] <= '9') && input[i] != '+' && input[i] != '*' && input[i] != '/' && input[i] != '-') {
      fprintf(stderr, "Invalid input: Only digits, '+', '-', '/', and '*' are allowed.\n");
      return 1;
    }
  }
  // do convert
  convertToZshMath(input, output);
  // Print result
  printf("%s\n", output);
  return 0;
}
