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
  if (set->count == set->capacity) {
    set->capacity = set->capacity == 0 ? 4 : set->capacity * 2;
    set->data = resize_array(set->data, sizeof(Coordinates), set->capacity / 2, set->capacity);
  }
  set->data[set->count++] = coord;
}

bool coordinates_equal(Coordinates a, Coordinates b) {
  return a.x == b.x && a.y == b.y;
}

bool is_in_map(Coordinates coords, Map map) {
  return coords.x >= 0 && coords.y >= 0 &&
       coords.x < (int)map.x_length && coords.y < (int)map.y_length;
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

void to_pairs(CoordinateSet *coordinates, CoordinateSet *pairs) {
  for (int i = 0; i < coordinates->count; i++) {
    for (int j = i + 1; j < coordinates->count; j++) {
      add_coordinate(pairs, coordinates->data[i]);
      add_coordinate(pairs, coordinates->data[j]);
    }
  }
}

void insert_new_antidotes(Coordinates *coords1, Coordinates *coords2, int dx, int dy,
              CoordinateSet *target, Map map) {
  int x_diff = abs(coords1->x - coords2->x);
  int y_diff = abs(coords1->y - coords2->y);
  int curr_x_diff = x_diff;
  int curr_y_diff = y_diff;
  while (1) {
    Coordinates new_antinode = {coords1->x + dx * curr_x_diff, coords1->y + dy * curr_y_diff};
    if (!is_in_map(new_antinode, map)) break;
    add_coordinate(target, new_antinode);
    curr_x_diff += x_diff;
    curr_y_diff += y_diff;
  }
}

void to_new_antinodes(Coordinates *coords1, Coordinates *coords2, CoordinateSet *new_antinodes, Map map) {
  int dx1, dy1, dx2, dy2;
  if (coords1->x > coords2->x && coords1->y > coords2->y) {
    dx1 = 1; dy1 = 1; dx2 = -1; dy2 = -1;
  } else if (coords1->x > coords2->x && coords1->y < coords2->y) {
    dx1 = 1; dy1 = -1; dx2 = -1; dy2 = 1;
  } else if (coords1->x < coords2->x && coords1->y > coords2->y) {
    dx1 = -1; dy1 = 1; dx2 = 1; dy2 = -1;
  } else {
    dx1 = -1; dy1 = -1; dx2 = 1; dy2 = 1;
  }
  insert_new_antidotes(coords1, coords2, dx1, dy1, new_antinodes, map);
  insert_new_antidotes(coords1, coords2, dx2, dy2, new_antinodes, map);
}

CoordinateSet to_antinodes(AntennaList *antennas, Map map) {
  CoordinateSet antinodes = {NULL, 0, 0};
  for (int i = 0; i < antennas->count; i++) {
    CoordinateSet pairs = {NULL, 0, 0};
    to_pairs(&antennas->data[i].coordinates, &pairs);
    for (int j = 0; j < pairs.count; j += 2) {
      Coordinates *coords1 = &pairs.data[j];
      Coordinates *coords2 = &pairs.data[j + 1];
      add_coordinate(&antinodes, *coords1);
      add_coordinate(&antinodes, *coords2);
      CoordinateSet new_antinodes = {NULL, 0, 0};
      to_new_antinodes(coords1, coords2, &new_antinodes, map);
      for (int k = 0; k < new_antinodes.count; k++) {
        if (is_in_map(new_antinodes.data[k], map)) {
          add_coordinate(&antinodes, new_antinodes.data[k]);
        }
      }
      free(new_antinodes.data);
    }
    free(pairs.data);
  }
  return antinodes;
}

void print_map(AntennaList *antennas, CoordinateSet *antinodes, Map map) {
  for (size_t y = 0; y < map.y_length; y++) {
    for (size_t x = 0; x < map.x_length; x++) {
      Coordinates coords = {x, y};
      char symbol = '.';
      for (int i = 0; i < antennas->count; i++) {
        for (int j = 0; j < antennas->data[i].coordinates.count; j++) {
          if (coordinates_equal(coords, antennas->data[i].coordinates.data[j])) {
            symbol = antennas->data[i].frequency;
            break;
          }
        }
      }
      for (int j = 0; j < antinodes->count; j++) {
        if (coordinates_equal(coords, antinodes->data[j])) {
          symbol = '#';
          break;
        }
      }
      printf("%c", symbol);
    }
    printf("\n");
  }
}

int solve(char **lines, int line_count) {
  Map map = to_map(lines, line_count);
  AntennaList antennas = to_antennas(lines, line_count);
  CoordinateSet antinodes = to_antinodes(&antennas, map);
  print_map(&antennas, &antinodes, map);
  int result = antinodes.count;
  for (int i = 0; i < antennas.count; i++) {
    free(antennas.data[i].coordinates.data);
  }
  free(antennas.data);
  free(antinodes.data);
  return result;
}

int main() {
  FILE *file = fopen("input.txt", "r");
  if (!file) {
    perror("Failed to open file");
    return EXIT_FAILURE;
  }
  char **lines = NULL;
  int line_count = 0, line_capacity = 0;
  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), file)) {
    size_t len = strlen(buffer);
    if (buffer[len - 1] == '\n') buffer[len - 1] = '\0';
    if (line_count == line_capacity) {
      line_capacity = line_capacity == 0 ? 10 : line_capacity * 2;
      lines = resize_array(lines, sizeof(char *), line_capacity / 2, line_capacity);
    }
    lines[line_count++] = strdup(buffer);
  }
  fclose(file);
  printf("%d\n", solve(lines, line_count));
  for (int i = 0; i < line_count; i++) {
    free(lines[i]);
  }
  free(lines);
  return 0;
}
