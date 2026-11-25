/**
 * Simple program to demonstrate OOP features.
 * Run with `java ./main.java`. Requires at least java version 25.
 * If you don't have java installed, you can run this program online at
 * `www.online-java.com`, make sure to select version 25.
 */

interface LivingBeing {
  public void sayHello();
}

class Cat implements LivingBeing {

  public Cat(int lives) {
    this.lives = lives;
  }

  public int lives;

  @Override
  public void sayHello() {
    IO.println("Meow, I'm a cat, I have %d lives!".formatted(this.lives));
  }
}

class Dog implements LivingBeing {

  public Dog(String name) {
    this.name = name;
  }

  public String name;

  @Override
  public void sayHello() {
    IO.println("Woof, my name is %s!".formatted(this.name));
  }
}

class ObiWan implements LivingBeing {
  @Override
  public void sayHello() {
    IO.println("Hello there!");
  }
}

void introduceAll(LivingBeing[] livingBeings) {
  for (int i = 0; i < livingBeings.length; i++) {
    livingBeings[i].sayHello();
  }
}

void main(String[] args) {
  var livingBeings = new LivingBeing[3];
  livingBeings[0] = new Cat(9);
  livingBeings[1] = new Dog("Laika");
  livingBeings[2] = new ObiWan();
  introduceAll(livingBeings);
}
