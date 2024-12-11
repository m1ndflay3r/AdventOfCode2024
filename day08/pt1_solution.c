#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
  int x;
  int y;
} Coordinates;

typedef struct {
  int x_length;
  int y_length;
} Map;

typedef struct {
  char frequency;
  Coordinates *coords;
  int count;
  int capacity;
} Antenna;

typedef struct {
  Antenna *antennas;
  int count;
  int capacity;
} AntennaList;

typedef struct {
  Coordinates *pairs;
  int count;
  int capacity;
} CoordinatePairs;

// dynamically resize array
void *resize_array(void *array, size_t element_size, int old_capacity, int new_capacity) {
  void *new_array = realloc(array, element_size * new_capacity);
  if (!new_array) {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }
  return new_array;
}

// dynamically add coordinate
void add_coordinate(Coordinates **coords, int *count, int *capacity, Coordinates coord) {
  if (*count == *capacity) {
    *capacity = *capacity == 0 ? 2 : *capacity * 2;
    *coords = resize_array(*coords, sizeof(Coordinates), *capacity / 2, *capacity);
  }
  (*coords)[(*count)++] = coord;
}

// dynamically add antenna
void add_antenna(AntennaList *list, char frequency) {
  if (list->count == list->capacity) {
    list->capacity = list->capacity == 0 ? 2 : list->capacity * 2;
    list->antennas = resize_array(list->antennas, sizeof(Antenna), list->capacity / 2, list->capacity);
  }
  Antenna antenna = {frequency, NULL, 0, 0};
  list->antennas[list->count++] = antenna;
}

bool coordinates_equal(Coordinates a, Coordinates b) {
  return a.x == b.x && a.y == b.y;
}

bool is_in_map(Coordinates coords, Map map) {
  return coords.x >= 0 && coords.y >= 0 &&
       coords.x < map.x_length && coords.y < map.y_length;
}

Map to_map(char **lines, int line_count) {
  Map map = {strlen(lines[0]), line_count};
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
        if (antennas.antennas[i].frequency == frequency) {
          add_coordinate(&antennas.antennas[i].coords, &antennas.antennas[i].count,
                   &antennas.antennas[i].capacity, (Coordinates){x, y});
          found = true;
          break;
        }
      }
      if (!found) {
        add_antenna(&antennas, frequency);
        add_coordinate(&antennas.antennas[antennas.count - 1].coords,
                 &antennas.antennas[antennas.count - 1].count,
                 &antennas.antennas[antennas.count - 1].capacity, (Coordinates){x, y});
      }
    }
  }
  return antennas;
}

CoordinatePairs to_pairs(Coordinates *coords, int count) {
  CoordinatePairs pairs = {NULL, 0, 0};
  for (int i = 0; i < count; i++) {
    for (int j = i + 1; j < count; j++) {
      add_coordinate(&pairs.pairs, &pairs.count, &pairs.capacity, coords[i]);
      add_coordinate(&pairs.pairs, &pairs.count, &pairs.capacity, coords[j]);
    }
  }
  return pairs;
}

void to_antinode_pair(Coordinates coords1, Coordinates coords2, Coordinates *antinode1, Coordinates *antinode2) {
  int x_diff = abs(coords1.x - coords2.x);
  int y_diff = abs(coords1.y - coords2.y);
  int dx1 = coords1.x < coords2.x ? -1 : 1;
  int dy1 = coords1.y < coords2.y ? -1 : 1;
  int dx2 = -dx1;
  int dy2 = -dy1;
  *antinode1 = (Coordinates){coords1.x + dx1 * x_diff, coords1.y + dy1 * y_diff};
  *antinode2 = (Coordinates){coords2.x + dx2 * x_diff, coords2.y + dy2 * y_diff};
}

bool contains_coordinate(Coordinates *coords, int count, Coordinates coord) {
  for (int i = 0; i < count; i++) {
    if (coordinates_equal(coords[i], coord)) {
      return true;
    }
  }
  return false;
}

void print_map(AntennaList *antennas, Coordinates *antinodes, int antinode_count, Map map) {
  for (int y = 0; y < map.y_length; y++) {
    for (int x = 0; x < map.x_length; x++) {
      Coordinates coords = {x, y};
      char symbol = '.';
      for (int i = 0; i < antennas->count; i++) {
        for (int j = 0; j < antennas->antennas[i].count; j++) {
          if (coordinates_equal(coords, antennas->antennas[i].coords[j])) {
            symbol = antennas->antennas[i].frequency;
            break;
          }
        }
      }
      for (int i = 0; i < antinode_count; i++) {
        if (coordinates_equal(coords, antinodes[i])) {
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
  Coordinates *antinodes = NULL;
  int antinode_count = 0, antinode_capacity = 0;
  for (int i = 0; i < antennas.count; i++) {
    CoordinatePairs pairs = to_pairs(antennas.antennas[i].coords, antennas.antennas[i].count);
    for (int j = 0; j < pairs.count; j += 2) {
      Coordinates antinode1, antinode2;
      to_antinode_pair(pairs.pairs[j], pairs.pairs[j + 1], &antinode1, &antinode2);
      if (is_in_map(antinode1, map) && !contains_coordinate(antinodes, antinode_count, antinode1)) {
        add_coordinate(&antinodes, &antinode_count, &antinode_capacity, antinode1);
      }
      if (is_in_map(antinode2, map) && !contains_coordinate(antinodes, antinode_count, antinode2)) {
        add_coordinate(&antinodes, &antinode_count, &antinode_capacity, antinode2);
      }
    }
    free(pairs.pairs);
  }
  print_map(&antennas, antinodes, antinode_count, map);
  // Clean up
  for (int i = 0; i < antennas.count; i++) {
    free(antennas.antennas[i].coords);
  }
  free(antennas.antennas);
  free(antinodes);
  return antinode_count;
}

int main() {
  FILE *file = fopen("input", "r");
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
    lines[line_count] = strdup(buffer);
    line_count++;
  }
  fclose(file);
  printf("result: %d\n", solve(lines, line_count));
  for (int i = 0; i < line_count; i++) {
    free(lines[i]);
  }
  free(lines);
  return 0;
}
