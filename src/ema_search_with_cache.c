#include "../lib/include/api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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

int process_file(int fd, const char *substring) {
  off_t offset = 0;
  int iteration = 0;
  char buffer[BUFFER_SIZE];

  while (1) {
    // Move the file cursor to the correct offset using c_lseek
    if (c_lseek(fd, offset, SEEK_SET) == (off_t)-1) {
      perror("Error seeking file");
      return EXIT_FAILURE;
    }

    ssize_t bytes_read = c_read(fd, buffer, BUFFER_SIZE);
    if (bytes_read <= 0) {
      break; // End of file or error
    }

    int index = search_substring(buffer, substring);
    if (index != -1) {
      printf("Found substring at position %lld\n", offset + index);
      break;
    }

    printf("Not Found substring at offset %lld\n", offset);

    iteration++;
    offset += bytes_read; // Move the offset forward by the number of bytes read
  }

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
    fprintf(stderr, "Substring is larger than buffer size\n");
    return EXIT_FAILURE;
  }

  // Open the file using c_open, instead of fopen
  int fd = c_open(filename);
  if (fd == -1) {
    perror("Error opening file");
    return EXIT_FAILURE;
  }

  clock_t total_start = clock();
  for (int r = 0; r < repeats; r++) {
    clock_t start = clock();
    if (process_file(fd, substring) != 0) {
      fprintf(stderr, "[%d] AHTUNG AHTUNG SOME ERROR OCCURRED\n", r);
    }
    printf("Execution time for repetition %d: %lf seconds\n\n", r,
           (double)(clock() - start) / CLOCKS_PER_SEC);
  }
  clock_t total_end = clock();

  c_close(fd); // Close the file using c_close instead of fclose

  printf("\n");
  printf(">>> Total execution time: %lf seconds <<<\n\n",
         (double)(total_end - total_start) / CLOCKS_PER_SEC);

  return EXIT_SUCCESS;
}
