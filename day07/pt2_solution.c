#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INITIAL_CAPACITY 10
#define RESULT_CAPACITY 1000  // Arbitrary large size to hold possible results

typedef struct {
  int test_value;
  int *operands;
  int operand_count;
} Equation;

Equation *equations = NULL;
int equation_count = 0;
int equation_capacity = 0;

// Function to ensure that the equations array has enough space
void ensure_capacity() {
  if (equation_count >= equation_capacity) {
    equation_capacity = equation_capacity == 0 ? INITIAL_CAPACITY : equation_capacity * 2;
    equations = realloc(equations, equation_capacity * sizeof(Equation));
    if (!equations) {
      perror("Failed to reallocate memory");
      exit(1);
    }
  }
}

// Function to parse input from the file
void parse_input() {
  FILE *file = fopen("input", "r");
  if (!file) {
    perror("Failed to find puzzle input! are you in the right directory?");
    exit(1);
  }
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    int test_value;
    int *operands = malloc(INITIAL_CAPACITY * sizeof(int));
    int operand_count = 0;
    // Parse test_value before the colon
    sscanf(line, "%d:", &test_value);
    // Parse operands after the colon
    char *operand_str = strchr(line, ':') + 1;
    char *token = strtok(operand_str, " \n");
    while (token != NULL) {
      if (operand_count >= INITIAL_CAPACITY) {
        operands = realloc(operands, (operand_count + 1) * sizeof(int));
        if (!operands) {
          perror("Failed to reallocate memory for operands");
          exit(1);
        }
      }
      operands[operand_count++] = atoi(token);
      token = strtok(NULL, " \n");
    }
    ensure_capacity();
    equations[equation_count].test_value = test_value;
    equations[equation_count].operand_count = operand_count;
    equations[equation_count].operands = operands;
    equation_count++;
  }
  fclose(file);
}

// Function to apply an operator between two integers
int apply_operator(int left, int right, char operator) {
  if (operator == '+') return left + right;
  if (operator == '*') return left * right;
  return left; // No operator
}

// Recursive function to generate all possible results
void get_possible_results(int *operands, int operand_count, int *results, int *result_count) {
  if (operand_count == 1) {
    // Base case: only one operand, return it
    if (*result_count < RESULT_CAPACITY) {
      results[*result_count] = operands[0];
      (*result_count)++;
    }
    return;
  }
  // Try all operators between the first two operands, and recursively process the rest
  for (int i = 0; i < operand_count - 1; i++) {
    for (int j = i + 1; j < operand_count; j++) {
      char operators[] = {'+', '*', '\0'};
      for (int op = 0; operators[op] != '\0'; op++) {
        int left_result = apply_operator(operands[i], operands[j], operators[op]);
        // Create a new operand array after applying the operator
        int new_operands[operand_count - 1];
        int k = 0;
        for (int m = 0; m < operand_count; m++) {
          if (m != i && m != j) {
            new_operands[k++] = operands[m];
          }
        }
        new_operands[k] = left_result;  // Add the result of applying the operator
        // Recurse on the new operands
        get_possible_results(new_operands, operand_count - 1, results, result_count);
      }
    }
  }
}

// Function to free the dynamically allocated memory
void free_memory() {
  for (int i = 0; i < equation_count; i++) {
    free(equations[i].operands);  // Free only the operands array
  }
  free(equations);  // Free the main equations array
}

int main() {
  clock_t start_time = clock();
  parse_input();
  int total_calibration_result = 0;
  for (int i = 0; i < equation_count; i++) {
    // Allocate memory for possible results dynamically for each equation
    int *possible_results = malloc(RESULT_CAPACITY * sizeof(int)); 
    if (!possible_results) {
      perror("Failed to allocate memory for possible_results");
      exit(1);
    }
    int result_count = 0;
    get_possible_results(equations[i].operands, equations[i].operand_count, possible_results, &result_count);
    for (int j = 0; j < result_count; j++) {
      if (possible_results[j] == equations[i].test_value) {
        total_calibration_result += equations[i].test_value;
        break;
      }
    }
    // Free the dynamically allocated result buffer
    free(possible_results);
  }
  clock_t end_time = clock();
  double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  printf("%d\n", total_calibration_result);
  // Free memory for all equations and operands after processing
  free_memory();
  return 0;
}
