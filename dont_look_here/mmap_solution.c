#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void panic(const char *message) {
  fprintf(stderr, "%s\n", message);
  exit(-1);
}

// Memory maps file_size bytes of file described by file_descriptor, and returns
// pointer to newly mapped memory. On error, returns NULL.
void *memory_map_file(int file_descriptor, size_t file_size) {
  void *memory_region = mmap(NULL, file_size, PROT_READ | PROT_WRITE,
                             MAP_SHARED, file_descriptor, 0);
  if (memory_region == MAP_FAILED) {
    return NULL;
  }
  return memory_region;
}

/// Gets file size of file described by file_descriptor, and returns it via the
/// *file_size_out pointer. On error, does nothing.
void get_file_size(int file_descriptor, size_t *file_size_out) {
  struct stat file_stats = {0};
  int stat_success = fstat(file_descriptor, &file_stats);
  if (stat_success == 0) {
    *file_size_out = file_stats.st_size;
  }
}

int main() {
  int file_descriptor = open("./foo.txt", O_RDWR);
  if (file_descriptor < 0) {
    panic("Cannot open file for read and write");
  }

  size_t file_size = 0;
  get_file_size(file_descriptor, &file_size);
  if (file_size == 0) {
    panic("Cannot determine size of file");
  }

  // This pointer now points to the file's contents!
  char *file_bytes = memory_map_file(file_descriptor, file_size);
  if (file_bytes == NULL) {
    panic("Cannot map file.");
  }

  // We can use it to read the file...
  printf("%s\n", file_bytes);

  // Or we can also use it to write to the file.
  file_bytes[0] = 'a';
  // take a look into the file after running this, you will see the first
  // character change to 'a'

  munmap(file_bytes, file_size);
  close(file_descriptor);
  return 0;
}
