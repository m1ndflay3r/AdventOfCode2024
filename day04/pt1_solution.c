#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1000

// Read grid from file into dynamic-alloc 2D array
int read_grid_from_file(char *filename, char ***grid, int *rows, int *cols) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    return 0;
  }
  *rows = 0;
  *cols = 0;
  char line[MAX_LINE_LENGTH];
  // First pass: Count rows, determine max col. length
  while (fgets(line, sizeof(line), file)) {
    (*rows)++;
    int len = strlen(line);
    if (len > *cols) {
      *cols = len - 1; // Subtract 1 for newline
    }
  }
  // Allocate grid mem
  *grid = (char **)malloc(*rows * sizeof(char *));
  for (int i = 0; i < *rows; i++) {
    (*grid)[i] = (char *)malloc((*cols + 1) * sizeof(char)); // +1 for null-term.
  }
  // Second pass: Read grid data into array
  rewind(file); // Reset file pointer
  int row = 0;
  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = '\0'; // Remove newline
    strcpy((*grid)[row], line);
    row++;
  }
  fclose(file);
  return 1;
}

// Check if keyword matches starting at specific position in given direction
int search_direction(char **grid, int rows, int cols, int start_x, int start_y, int dx, int dy, const char *keyword) {
  int keyword_len = strlen(keyword);
  // Check if out of bounds
  for (int i = 0; i < keyword_len; i++) {
    int x = start_x + i * dx;
    int y = start_y + i * dy;
    // If out of bounds, return 0 (invalid search)
    if (x < 0 || x >= rows || y < 0 || y >= cols) {
      return 0;
    }
    // If char not match, return 0
    if (grid[x][y] != keyword[i]) {
      return 0;
    }
  }
  return 1;
}

// search logic
int search_grid(char **grid, int rows, int cols, const char *keyword) {
  int total_count = 0;
  // Directions: (dx, dy) pairs (right, down, left, up, etc.)
  int directions[8][2] = {
    { 0, 1}, // right
    { 0, -1}, // left
    { 1, 0}, // down
    { -1, 0}, // up
    { 1, 1}, // down-right
    { -1, -1}, // up-left
    { 1, -1}, // down-left
    { -1, 1} // up-right
  };
  // Iterate over each cell, search each direction
  for (int x = 0; x < rows; x++) {
    for (int y = 0; y < cols; y++) {
      for (int i = 0; i < 8; i++) {
        int dx = directions[i][0];
        int dy = directions[i][1];
        if (search_direction(grid, rows, cols, x, y, dx, dy, keyword)) {
          total_count++;
        }
      }
    }
  }
  return total_count;
}

int main() {
  char **grid = NULL;
  int rows, cols;
  const char *keyword = "XMAS";
  int total_count;
  // Read grid from file
  if (!read_grid_from_file("input", &grid, &rows, &cols)) {
    printf("Failed to find puzzle input! Are you in the right directory?\n");
    return 1;
  }
  // Search grid
  total_count = search_grid(grid, rows, cols, keyword);
  printf("%d\n", total_count);
  // Free memory
  for (int i = 0; i < rows; i++) {
    free(grid[i]);
  }
  free(grid);
  return 0;
}
