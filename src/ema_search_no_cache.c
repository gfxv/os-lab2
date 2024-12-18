#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 128

int search_substring(const char *buffer, const char *substring) {
    int i, j;
    if (substring[0] == '\0') {
        return -1;
    }
    for (i = 0; buffer[i] != '\0'; i++) {
        for (j = 0; substring[j] != '\0'; j++) {
            if (buffer[i + j] != substring[j]) {
                break;
            }
        }
        if (substring[j] == '\0') {
            return i; 
        }
    }
    return -1;
}

int process_file(const char *filename, const char *substring) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening file");
    return EXIT_FAILURE;
  }

  int iteration = 0;
  char buffer[BUFFER_SIZE];
  while (fgets(buffer, BUFFER_SIZE, file)) {
    int index = search_substring(buffer, substring);
    if (index != -1) {
      printf("Found substring at position %d\n", iteration * BUFFER_SIZE + index);
      break;
    }
    iteration++;
  }
  fclose(file);

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: %s <filename> <substring> <repeats>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *filename = argv[1];
  const char *substring = argv[2];
  size_t repeats = atoi(argv[3]);

  if (strlen(substring) > BUFFER_SIZE) {
    fprintf(stderr, "Substring is larger than buffer size");
  }

  clock_t total_start = clock();
  for (int r = 0; r < repeats; r++) {
    clock_t start = clock();
    if (process_file(filename, substring) != 0) {
      fprintf(stderr, "[%d] AHTUNG AHTUNG SOME ERROR OCCURRED\n", r);
    }
    printf("Execution time: %lf seconds\n\n", (double) (clock() - start) / CLOCKS_PER_SEC);
  }
  clock_t total_end = clock();

  printf("\n");
  printf(">>> Total execution time: %lf seconds <<<\n\n", (double) (total_end - total_start) / CLOCKS_PER_SEC);

  return EXIT_SUCCESS;
}
