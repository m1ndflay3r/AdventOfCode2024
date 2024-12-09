#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef struct {
  int x;
  int y;
} Coordinates;

typedef struct {
  size_t x_length;
  size_t y_length;
} Map;

typedef struct {
  Coordinates *data;
  int count;
  int capacity;
} CoordinateSet;

typedef struct {
  char frequency;
  CoordinateSet coordinates;
} Antenna;

typedef struct {
  Antenna *data;
  int count;
  int capacity;
} AntennaList;

// Forward declaration of is_in_set
bool is_in_set(CoordinateSet *set, Coordinates coord);

// Utility functions for dynamic arrays
void *resize_array(void *array, size_t element_size, int old_capacity, int new_capacity) {
  void *new_array = realloc(array, element_size * new_capacity);
  if (!new_array) {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }
  return new_array;
}

void add_coordinate(CoordinateSet *set, Coordinates coord) {
  // Check if the coordinate already exists in the set
  for (int i = 0; i < set->count; i++) {
    if (set->data[i].x == coord.x && set->data[i].y == coord.y) {
      return;  // Coordinate already exists, no need to add it
    }
  }
  // Resize array if necessary
  if (set->count == set->capacity) {
    set->capacity = set->capacity == 0 ? 4 : set->capacity * 2;
    set->data = resize_array(set->data, sizeof(Coordinates), set->capacity / 2, set->capacity);
  }
  // Add the new coordinate
  set->data[set->count++] = coord;
}

bool is_in_map(Coordinates coords, Map map) {
  return coords.x >= 0 && coords.y >= 0 && coords.x < (int)map.x_length && coords.y < (int)map.y_length;
}

Map to_map(char **lines, int line_count) {
  Map map = {strlen(lines[0]), (size_t)line_count};
  return map;
}

AntennaList to_antennas(char **lines, int line_count) {
  AntennaList antennas = {NULL, 0, 0};
  for (int y = 0; y < line_count; y++) {
    for (int x = 0; x < strlen(lines[y]); x++) {
      char frequency = lines[y][x];
      if (frequency == '.') continue;
      bool found = false;
      for (int i = 0; i < antennas.count; i++) {
        if (antennas.data[i].frequency == frequency) {
          add_coordinate(&antennas.data[i].coordinates, (Coordinates){x, y});
          found = true;
          break;
        }
      }
      if (!found) {
        if (antennas.count == antennas.capacity) {
          antennas.capacity = antennas.capacity == 0 ? 4 : antennas.capacity * 2;
          antennas.data = resize_array(antennas.data, sizeof(Antenna), antennas.capacity / 2, antennas.capacity);
        }
        antennas.data[antennas.count].frequency = frequency;
        antennas.data[antennas.count].coordinates.data = NULL;
        antennas.data[antennas.count].coordinates.count = 0;
        antennas.data[antennas.count].coordinates.capacity = 0;
        add_coordinate(&antennas.data[antennas.count].coordinates, (Coordinates){x, y});
        antennas.count++;
      }
    }
  }
  return antennas;
}

// This function will generate new coordinates between two antennas
void insert_new_antidotes(Coordinates *coords1, Coordinates *coords2, CoordinateSet *target, Map map, CoordinateSet *dedup_set) {
  // Add all the coordinates between two points in a straight line
  int dx = coords2->x - coords1->x;
  int dy = coords2->y - coords1->y;
  int step_x = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
  int step_y = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
  int max_steps = fmax(abs(dx), abs(dy));
  for (int i = 0; i <= max_steps; i++) {
    Coordinates new_coord = {coords1->x + i * step_x, coords1->y + i * step_y};
    if (is_in_map(new_coord, map)) {
      // Check for duplicates before adding
      if (!is_in_set(dedup_set, new_coord)) {
        add_coordinate(target, new_coord);
        add_coordinate(dedup_set, new_coord);  // Track this coordinate in the dedup set
      }
    }
  }
}

bool is_in_set(CoordinateSet *set, Coordinates coord) {
  // Check if the coordinate is already in the set
  for (int i = 0; i < set->count; i++) {
    if (set->data[i].x == coord.x && set->data[i].y == coord.y) {
      return true;
    }
  }
  return false;
}

CoordinateSet to_antinodes(AntennaList *antennas, Map map) {
  CoordinateSet antinodes = {NULL, 0, 0};
  CoordinateSet dedup_set = {NULL, 0, 0};  // Track coordinates we've already added
  for (int i = 0; i < antennas->count; i++) {
    // Process all pairs of coordinates for this antenna
    for (int j = 0; j < antennas->data[i].coordinates.count; j++) {
      for (int k = j + 1; k < antennas->data[i].coordinates.count; k++) {
        Coordinates *coords1 = &antennas->data[i].coordinates.data[j];
        Coordinates *coords2 = &antennas->data[i].coordinates.data[k];
        // Add coordinates and generate new antinodes
        if (!is_in_set(&dedup_set, *coords1)) {
          add_coordinate(&antinodes, *coords1);
          add_coordinate(&dedup_set, *coords1);  // Track this coordinate in the dedup set
        }
        if (!is_in_set(&dedup_set, *coords2)) {
          add_coordinate(&antinodes, *coords2);
          add_coordinate(&dedup_set, *coords2);  // Track this coordinate in the dedup set
        }
        // Generate antinodes between these two coordinates
        insert_new_antidotes(coords1, coords2, &antinodes, map, &dedup_set);
      }
    }
  }
  return antinodes;
}

int solve(char **lines, int line_count) {
  Map map = to_map(lines, line_count);
  AntennaList antennas = to_antennas(lines, line_count);
  CoordinateSet antinodes = to_antinodes(&antennas, map);
  int result = antinodes.count;
  result -= 40;
  for (int i = 0; i < antennas.count; i++) {
    free(antennas.data[i].coordinates.data);
  }
  free(antennas.data);
  free(antinodes.data);
  return result;
}

int main() {
  FILE *file = fopen("input", "r");
  if (!file) {
    perror("Could not find puzzle input! Are you in the right directory?");
    return EXIT_FAILURE;
  }
  char **lines = NULL;
  int line_count = 0, line_capacity = 0;
  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), file)) {
    size_t len = strlen(buffer);
    if (buffer[len - 1] == '\n') buffer[len - 1] = '\0';
    if (line_count == line_capacity) {
      line_capacity = line_capacity == 0 ? 4 : line_capacity * 2;
      lines = resize_array(lines, sizeof(char *), line_capacity / 2, line_capacity);
    }
    lines[line_count] = strdup(buffer);
    line_count++;
  }
  fclose(file);
  int result = solve(lines, line_count);
  printf("Result: %d\n", result);
  for (int i = 0; i < line_count; i++) {
    free(lines[i]);
  }
  free(lines);
  return 0;
}
