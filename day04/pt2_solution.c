#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// check if "X-(MAS &&|| SAM)" pattern found at target position
bool dayfour_is_match(char **lines, size_t rows, size_t cols, size_t y, size_t x) {
  if (x + 3 > cols || y + 3 > rows) {
    return false;
  }
  char dayfour_check[3][3] = {
    {lines[y][x], lines[y][x + 1], lines[y][x + 2]},
    {lines[y + 1][x + 1], 0, 0},
    {lines[y + 2][x], lines[y + 2][x + 1], lines[y + 2][x + 2]}
  };
  return (
    (dayfour_check[0][0] == 'M' && dayfour_check[0][2] == 'M' && dayfour_check[1][0] == 'A' && dayfour_check[2][0] == 'S' && dayfour_check[2][2] == 'S') ||
    (dayfour_check[0][0] == 'M' && dayfour_check[0][2] == 'S' && dayfour_check[1][0] == 'A' && dayfour_check[2][0] == 'M' && dayfour_check[2][2] == 'S') ||
    (dayfour_check[0][0] == 'S' && dayfour_check[0][2] == 'S' && dayfour_check[1][0] == 'A' && dayfour_check[2][0] == 'M' && dayfour_check[2][2] == 'M') ||
    (dayfour_check[0][0] == 'S' && dayfour_check[0][2] == 'M' && dayfour_check[1][0] == 'A' && dayfour_check[2][0] == 'S' && dayfour_check[2][2] == 'M')
  );
}

// Determine grid pattern count
unsigned int count_match(char **lines, size_t rows, size_t cols) {
  unsigned int count = 0;
  for (size_t y = 0; y < rows; ++y) {
    for (size_t x = 0; x < cols; ++x) {
      if (dayfour_is_match(lines, rows, cols, y, x)) {
        ++count;
      }
    }
  }
  return count;
}

// Solve logic
unsigned int fetch_result(char **lines, size_t rows, size_t cols) {
  return count_match(lines, rows, cols);
}

int main() {
  // Fetch puzzle input
  FILE *file = fopen("input", "r");
  if (!file) {
    perror("Failed to find puzzle input! Are you in the right directory?");
    return EXIT_FAILURE;
  }
  char **lines = NULL;
  size_t rows = 0, cols = 0;
  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), file)) {
    size_t len = strlen(buffer);
    if (buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
      --len;
    }
    if (len > cols) {
      cols = len;
    }
    lines = realloc(lines, (rows + 1) * sizeof(char *));
    lines[rows] = malloc(len + 1);
    strcpy(lines[rows], buffer);
    ++rows;
  }
  fclose(file);
  unsigned int result = fetch_result(lines, rows, cols);
  printf("%u\n", result);
  // Cleanup (memfree)
  for (size_t i = 0; i < rows; ++i) {
    free(lines[i]);
  }
  free(lines);
  return 0;
}
