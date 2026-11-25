/*
 * Let's re-implement the main.java file in C! Including polymorphism, of
 * course. Most OOP languages, including the JVM, actually do something like
 * this behind the scenes.
 *
 * */

#include <stddef.h>
#include <stdio.h>

// This is our interface. We wish to override its methods. But of course, in C,
// there are no member functions or methods in structs. So for that, we use a
// common trick employed by almost all OOP languages behind the scenes, called a
// virtual table pointer.
struct LivingBeing {
  // This pointer points to an array of method pointers
  // Each method is a function pointer in this array
  // In our case, of course, there is just one method
  // Since we don't know what the method signature is, it's a void**.
  void **virtual_table;
};

// Let's typedef the signature of the sayHello function
typedef void (*sayHelloSignature)(struct LivingBeing *this);
// SAY_HELLO_OFFSET records that sayHello will be the first (so at index 0)
// method of the table.
const size_t SAY_HELLO_OFFSET = 0;

// Here are the three classes. No inheritance in C, so they just accept the base
// as a member field.
struct Cat {
  struct LivingBeing base;
  int lives;
};

struct Dog {
  struct LivingBeing base;
  const char *name;
};

struct ObiWan {
  struct LivingBeing base;
};

// Now for the implementations of the actual methods.
// Since there are no methods in C, we need to explicitly pass *this.
void sayHelloCat(struct Cat *this) {
  printf("Meow, I'm a cat, I have %i lives!\n", this->lives);
}

void sayHelloDog(struct Dog *this) {
  printf("Woof, my name is %s!\n", this->name);
}

void sayHelloObiWan(struct ObiWan *this) { printf("Hello there!\n"); }

// And let's make the method tables. As promised, a pointer to the sayHello
// function will be at index 0 in each of the tables.
void *CatVtable[] = {&sayHelloCat};
void *DogVtable[] = {&sayHelloDog};
void *ObiWanVtable[] = {&sayHelloObiWan};

// And now the POLYMORPHIC method!
// Accept any living being as argument.
void sayHello(struct LivingBeing *this) {
  // We know that every living being has a virtual_table
  // We get the appropriate sayHello method out of that virtual table
  sayHelloSignature sayHelloMethod = this->virtual_table[SAY_HELLO_OFFSET];
  // And now we can call it!
  sayHelloMethod(this);
}

void introduceAll(struct LivingBeing **livingBeings, size_t count) {
  // Pointer-pointer here,           ^^
  // because this is an array of pointers.
  for (size_t i = 0; i < count; ++i) {
    // Polymorphism go BRRRRR
    sayHello(livingBeings[i]);
  }
}

int main() {
  // Let's make the three objects. Remember to give them their proper vtable
  // pointer.
  struct Cat cat = {CatVtable, 9};
  struct Dog dog = {DogVtable, "Laika"};
  struct ObiWan obiWan = {ObiWanVtable};

  // Casting to a pointer to the first field of the struct is explicitly allowed
  // by C standard.
  struct LivingBeing *livingBeings[3] = {(struct LivingBeing *)&cat,
                                         (struct LivingBeing *)&dog,
                                         (struct LivingBeing *)&obiWan};
  introduceAll(livingBeings, 3);
}
