#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

// Define hash set structure (coord. pairs)
typedef struct {
  int x;
  int y;
} Coord;

typedef struct {
  Coord *coords;
  size_t size;
  size_t capacity;
} HashSet;

void hashset_init(HashSet *set) {
  set->size = 0;
  set->capacity = 16;
  set->coords = malloc(set->capacity * sizeof(Coord));
  if (!set->coords) {
    perror("Failed allocate hash set mem");
    exit(EXIT_FAILURE);
  }
}

void hashset_free(HashSet *set) {
  free(set->coords);
}

bool hashset_contains(const HashSet *set, Coord coord) {
  for (size_t i = 0; i < set->size; ++i) {
    if (set->coords[i].x == coord.x && set->coords[i].y == coord.y) {
      return true;
    }
  }
  return false;
}

void hashset_insert(HashSet *set, Coord coord) {
  if (!hashset_contains(set, coord)) {
    if (set->size == set->capacity) {
      set->capacity *= 2;
      set->coords = realloc(set->coords, set->capacity * sizeof(Coord));
      if (!set->coords) {
        perror("Failed reallocate hash set mem");
        exit(EXIT_FAILURE);
      }
    }
    set->coords[set->size++] = coord;
  }
}

void get_reachable_nines(
  uint32_t **map,
  size_t rows,
  size_t cols,
  uint32_t head,
  Coord head_location,
  HashSet *nine_locations
) {
  if (head == 9) {
    hashset_insert(nine_locations, head_location);
    return;
  }
  size_t head_x = head_location.x;
  size_t head_y = head_location.y;
  int dx[] = {-1, 1, 0, 0};
  int dy[] = {0, 0, -1, 1};
  for (int i = 0; i < 4; ++i) {
    int next_x = head_x + dx[i];
    int next_y = head_y + dy[i];
    if (next_x >= 0 && next_x < cols && next_y >= 0 && next_y < rows) {
      uint32_t next_head = map[next_y][next_x];
      if ((int32_t)(next_head - head) == 1) {
        Coord next_location = {next_x, next_y};
        get_reachable_nines(map, rows, cols, next_head, next_location, nine_locations);
      }
    }
  }
}

int main() {
  size_t rows = 0, cols = 0;
  uint32_t **map = malloc(1000 * sizeof(uint32_t *));
  if (!map) {
    perror("Failed allocate map mem");
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
    perror("Cannot find puzzle input! Are you in the right directory?");
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
  Coord *trailhead_locations = malloc(rows * cols * sizeof(Coord));
  if (!trailhead_locations) {
    perror("Failed allocate trailhead locations mem");
    return EXIT_FAILURE;
  }
  size_t trailhead_count = 0;
  for (size_t y = 0; y < rows; ++y) {
    for (size_t x = 0; x < cols; ++x) {
      if (map[y][x] == 0) {
        trailhead_locations[trailhead_count++] = (Coord){x, y};
      }
    }
  }
  size_t answer = 0;
  for (size_t i = 0; i < trailhead_count; ++i) {
    HashSet nine_locations;
    hashset_init(&nine_locations);
    Coord head_location = trailhead_locations[i];
    uint32_t head = map[head_location.y][head_location.x];
    get_reachable_nines(map, rows, cols, head, head_location, &nine_locations);
    answer += nine_locations.size;
    hashset_free(&nine_locations);
  }
  printf("%zu\n", answer);
  free(trailhead_locations);
  for (size_t i = 0; i < 1000; ++i) {
    free(map[i]);
  }
  free(map);
  return EXIT_SUCCESS;
}
