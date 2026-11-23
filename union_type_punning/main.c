#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* DISCLAIMER: The explanations I give are not 100% standard-compliant and.
 *
 *
 * Problem: Sometimes, we also want to reinterpret the same
 * binary data as a variable of another type.
 *
 * Traditional solution: Historically, the fact that the union members occupy
 * the same space in memory was used to achieve this purpose
 */

// Traditional use of unions

union FloatOrInt {
  float f;
  int i;
};

float reinterpret_int_as_float_incorrectly(int num) {
  union FloatOrInt float_or_int;
  float_or_int.i = num;
  // since float_or_int.i occupies the same memory as float_or_int.f, reading
  // from float_or_int.f gives us a float with the same binary data as the
  // float_or_int.i
  return float_or_int.f;
  // This is called "union type punning".
}

/*
 * WARNING: Even though such code is very common and is logically reasonable, it
 * is technically, according to the standard, *UNDEFINED BEHAVIOR* in most
 * cases.
 *
 * Reason: To be able to optimize code more aggressively, the compiler generally
 * (there are just 3 exceptions) assumes the following promise to be true:
 *
 * > The type of an object at any given memory address is unique and does not
 * > change for the lifetime of the object.
 *
 * So by assigning num to float_or_int.i, we begin the lifetime of an integer at
 * the address of the union &float_or_int. Then by reading float_or_int.f we
 * read from the same address as if it were a float, which violates the promise,
 * and is therefore undefined behavior.
 *
 * This is a common occurence in C/C++: To be able to optimize aggressively, the
 * compiler assumes some promises. If you break them, undefined behavior occurs.
 *
 * In a union, when we assign to a member, we make that member "active" and
 * begin its lifetime. Assigning to another member ends the lifetime of the
 * previously active one, and makes the new one active.
 **/

void proper_union_usage() {
  union FloatOrInt float_or_int;
  // Now "i" is the active member.
  float_or_int.i = 42;
  // Reading from the active integer is OK!
  printf("The integer: %i", float_or_int.i);
  // At address &float_or_int there is an integer, so this is OK!
  int *int_addr = (int *)&float_or_int;
  // Since int_addr points to an integer, we can legally read from it.
  printf("The integer: %i", *int_addr);
  // Assigning to "f" ends the lifetime of "i" and begins the lifetime of "f",
  // making it active
  float_or_int.f = 6.9;
  // Reading from the active float is OK!
  printf("The float: %f", float_or_int.f);
  // At address &float_or_int there is a float now, so this is OK!
  float *float_addr = (float *)&float_or_int;
  // Since float_addr points to a float, we can legally read from it.
  printf("The float: %f", *float_addr);

  // Note that even though the addresses int_addr, float_addr and &float_or_int
  // are the same location in memroy, reading from int_addr now would be
  // undefined behavior.
  //
  //                           vvvvvvvvv  undefined behavior
  // printf("The integer: %i", *int_addr);
  //
  // This is because at this address, there is a float now, which is the active
  // member of the union.
}

/*
 * TL;DR:
 *
 * Always make sure that your pointers point to the actual type at that memory
 * address. Unions allow you to store variables of different types at the same
 * address. But they *don't* allow you to get a pointer of a different type to
 * an already existing variable.
 */

/*
 * So how to do it properly?
 *
 * If we want to reinterpret the raw binary of an object into as a different
 * type, we have two valid options:
 * 1. Use two actually different addresses.
 * 2. Use one of the 3 mentioned exceptions.
 *
 * I will demonstrate option 1 because it is the easier and safer one.
 **/

float reinterpret_int_as_float_properly(int num) {
  // Since we cannot re-use the same address for an int and a float, let's make
  // another float variable
  float new_variable;
  // Now let's memcpy over the raw binary.
  // Don't worry, the compiler will optimize this away.
  memcpy(&new_variable, &num, sizeof(num));
  // now new_variable contains the same binary as num, but at a different
  // address! So no undefined behavior! Yay!
  return new_variable;
}

/*
 * But wait, no union? YES! To reinterpret the raw binary of a variable as a
 * different type, we MUST formally use a DIFFERENT address. So a union is not
 * appropriate here. Instead the correct tool for the job is actually
 * good old memcpy.
 */

// To satisfy the exercise, here are the proper implementations, once again,
// without unions, because this is not what unions are supposed to be used for.
// The right tool for the job here is memcpy.

uint32_t float_bits(float value) {
  uint32_t result;
  memcpy(&result, &value, sizeof(value));
  return result;
}

float bits_to_float(uint32_t bits) {
  float result;
  memcpy(&result, &bits, sizeof(bits));
  return result;
}


#ifndef EXERCISE_TEST
int main(void) {
  printf("float_bits(1.0f) = 0x%08X\n", (unsigned)float_bits(1.0f));
  printf("bits_to_float(0x40490FDB) = %.8g\n",
         (double)bits_to_float(0x40490FDBu));
  return 0;
}
#endif

/* Curiosity killed the cat:
 *
 * What are the three exceptions, you wonder?
 *
 * The following types can be used to read from *any* address whatsoever:
 *
 * char*
 * signed char*
 * unsigned char*
 *
 * In C++ the "special" types are
 *
 * char*
 * unsigned char*
 * std::byte*
 *
 * So the following would be allowed.
 * */

union IntOrChars {
  int i;
  char c[4];
};

void legal_union_abuse() {
  int num = 0x48692121;
  union IntOrChars int_or_chars;
  int_or_chars.i = num;
  char *char_ptr = int_or_chars.c;
  //                                  v no \0 at the end here!
  printf("Integer %i as characters: %.4s\n", num, char_ptr);
  // This was only allowed with char*, signed char* or unsigned char*
  // We have effectively "punned" int with char[4].
}

int main_backup() {
  legal_union_abuse();
  return 0;
}
