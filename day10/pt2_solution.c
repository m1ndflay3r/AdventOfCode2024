#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

size_t calculate_map_rating(
  uint32_t **map,
  size_t rows,
  size_t cols,
  uint32_t head,
  size_t head_x,
  size_t head_y
) {
  if (head == 9) {
    return 1;
  }
  size_t rating = 0;
  int dx[] = {-1, 1, 0, 0};
  int dy[] = {0, 0, -1, 1};
  for (int i = 0; i < 4; ++i) {
    int next_x = head_x + dx[i];
    int next_y = head_y + dy[i];
    if (next_x >= 0 && next_x < cols && next_y >= 0 && next_y < rows) {
      uint32_t next_head = map[next_y][next_x];
      if ((int32_t)(next_head - head) == 1) {
        rating += calculate_map_rating(map, rows, cols, next_head, next_x, next_y);
      }
    }
  }
  return rating;
}

int main() {
  size_t rows = 0, cols = 0;
  uint32_t **map = malloc(1000 * sizeof(uint32_t *));
  if (!map) {
    perror("Failed to allocate map mem");
    return EXIT_FAILURE;
  }
  for (size_t i = 0; i < 1000; ++i) {
    map[i] = malloc(1000 * sizeof(uint32_t));
    if (!map[i]) {
      perror("Failed allocate map row mem");
      return EXIT_FAILURE;
    }
  }
  FILE *file = fopen("input", "r");
  if (!file) {
    perror("Failed to find puzzle input! Are you in the right directory?");
    return EXIT_FAILURE;
  }
  char line[1024];
  while (fgets(line, sizeof(line), file)) {
    size_t col = 0;
    for (char *ch = line; *ch && *ch != '\n'; ++ch) {
      map[rows][col++] = *ch - '0';
    }
    cols = col;
    ++rows;
  }
  fclose(file);
  size_t trailhead_count = 0;
  size_t answer = 0;
  for (size_t y = 0; y < rows; ++y) {
    for (size_t x = 0; x < cols; ++x) {
      if (map[y][x] == 0) {
        answer += calculate_map_rating(map, rows, cols, map[y][x], x, y);
        ++trailhead_count;
      }
    }
  }
  printf("%zu\n", answer);
  for (size_t i = 0; i < 1000; ++i) {
    free(map[i]);
  }
  free(map);
  return EXIT_SUCCESS;
}
