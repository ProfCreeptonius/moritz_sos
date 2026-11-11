#include <stdio.h>
#include <stdlib.h>

void panic(const char *message) {
  fprintf(stderr, "%s\n", message);
  exit(-1);
}

char *resize_my_buffer(char *buffer, size_t new_size) {
  char *new_buf = realloc(buffer, new_size);
  if (new_buf == NULL) {
    panic("Memory allocation failure on realloc");
  }
  return new_buf;
}

char *readline(FILE *fp) {
  int offset = 0;
  int bufsize = 4;
  char *buf = malloc(bufsize);

  if (buf == NULL)
    panic("Memory allocation failure on malloc");

  // Best practice: always initialize variables to safe values
  int current_char = '\n';

  for (;;) {
    current_char = fgetc(fp); // int because of legacy, should be char

    // Am at end of line or end of file?
    if (current_char == '\n' || current_char == EOF)
      break;

    // Filled up my buffer?
    if (offset == bufsize - 1) {
      bufsize *= 2;
      buf = resize_my_buffer(buf, bufsize);
    }

    buf[offset] = current_char;
    ++offset;
  }

  // Nothing happened in the for loop, it's an empty file?
  if (current_char == EOF && offset == 0) {
    free(buf);
    return NULL;
  }

  // Buffer is bigger than it needs to be?
  if (offset < bufsize - 1) {
    buf = resize_my_buffer(buf, offset + 1); // +1 for NUL terminator
  }

  buf[offset] = '\0';

  return buf;
}

int main(void) {
  FILE *fp = fopen("foo.txt", "r");
  if (fp == NULL) {
    panic("Cannot open file foo.txt for reading");
  }

  for (;;) {
    char *line = readline(fp);
    if (line == NULL)
      break;
    printf("%s\n", line);
    free(line); // Beej forgot to free his lines
  }

  fclose(fp);
}
