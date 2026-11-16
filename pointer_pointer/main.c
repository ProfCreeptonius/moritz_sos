#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Concept: Out parameters
 *
 * Problem: Sometimes in C we wish to return multiple return values
 * That is not possible by the language design.
 *
 * Solution: Instead of returning multiple values, we take multiple
 * pointer parameters, and then insert the return value at the address
 * pointed to by those parameters.
 *
 * These parameters which actually play the role of a return value are called
 * "Out parameters"
 */

// Example returning multiple ints:

void get_magic_numbers(int *magic_no_1, int *magic_no_2, int *magic_no_3) {
  *magic_no_1 = 42;
  *magic_no_2 = 69;
  *magic_no_3 = 1337;
  // We have effectively returned three values.
}

/*
 * Of course, it would also be possible to return the multiple values in a
 * struct, but that requires an additional struct definition.
 */

struct MagicNumbers {
  int magic_no_1;
  int magic_no_2;
  int magic_no_3;
};

struct MagicNumbers get_magic_numbers_struct() {
  struct MagicNumbers result;
  result.magic_no_1 = 42;
  result.magic_no_2 = 69;
  result.magic_no_3 = 1337;
  return result;
}

/*
 * Here is how we would use both of these functions:
 */

// Example: using functions

void use_magic_numbers() {
  int magic_no_1;
  int magic_no_2;
  int magic_no_3;
  get_magic_numbers(&magic_no_1, &magic_no_2, &magic_no_3);
  // do something with magic numbers
}

void use_magic_numbers_struct() {
  struct MagicNumbers magic_numbers = get_magic_numbers_struct();
  // do something with magic numbers
}

/*
 * This is what strtol does
 *
 * long strtol(const char* string, const char** bad_char_address, int base);
 * ^^^^ value returned here        ^^^^^^^^^^^^^^^^^^^^^^^ first bad character
 * address returned here!
 *
 * Let's re-implement it ourselves!
 * NOTE: this is a simplified implementation which is not exactly compliant with
 * the ISO C99 standard.
 */

int is_digit(char ch) {
  // Magic char trickery.
  // See `man ascii` for an ascii table, that should help you understand why
  // this works.
  return ch >= '0' && ch <= '9';
}

long char_to_long(char ch) { 
  // More ascii trickery. See the ascii table, that should help you understand this
  return (long)ch - '0';
}

long my_strtol(const char *string, const char **bad_char_address, int base) {
  long result = 0;
  long sign = 1;
  if (string[0] == '-') {
    sign = -1;
    ++string;
  }
  const char* current_char_ptr = string;  
  while (*current_char_ptr != '\0') {
    if (!is_digit(*current_char_ptr)) {
      break;
    }
    result += char_to_long(*current_char_ptr);
    result *= base;
    ++current_char_ptr;
  }
  if (bad_char_address != NULL) {
    *bad_char_address = current_char_ptr;
  }
  return result * sign / base;
}

int test_my_strtol(const char *string, int base) {
  printf("Testing string: %s, Base: %i...\n", string, base);
  const char *my_bad_char = NULL;
  long my_value = my_strtol(string, &my_bad_char, base);

  const char *stdlib_bad_char = NULL;
  // Ignore the compiler warning here. It is a relic of a much older C standard,
  // and it will be irrelevant once you get to C++ :)
  long stdlib_value = strtol(string, &stdlib_bad_char, base);

  if (my_bad_char == NULL && stdlib_bad_char == NULL) {
    return my_value == stdlib_value;
  }
  return my_bad_char == stdlib_bad_char && my_value == stdlib_value;
}

int main() {
  assert(test_my_strtol("69", 10));
  assert(test_my_strtol("101101", 2));
  assert(test_my_strtol("42hahaha", 5));
  assert(test_my_strtol("aboba", 2));
  printf("All tests passed!\n");
}
