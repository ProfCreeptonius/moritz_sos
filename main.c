#include <stdio.h>
#include <stdlib.h>

void panic(const char *message) {
  fprintf(stderr, "%s", message);
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
  char *buf;
  int current_char; // int because of legacy, this should be char.

  buf = malloc(bufsize);

  if (buf == NULL)
    panic("Memory allocation failure on malloc");

  for (;;) {
    current_char = fgetc(fp);
    if (current_char == '\n' || current_char == EOF)
      break;

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

  if (offset < bufsize - 1) {
    buf = resize_my_buffer(buf, offset + 1); // +1 for NUL terminator
  }

  buf[offset] = '\0';

  return buf;
}

int main(void) {
  FILE *fp = fopen("foo.txt", "r");

  char *line;

  for (;;) {
    char *line = readline(fp);
    if (line == NULL) break;
    printf("%s\n", line);
  }

  fclose(fp);
}
