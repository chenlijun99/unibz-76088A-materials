# Step: FreeRTOS

Goal: from the base project, we want our firmware to have the following behaviour:

* It keeps the LED blinking.
* Allow the user to change the color of the LED on-the-fly.

## 0

The base project. Now, enough with the chores. Let's look at the code.

From the code logic POV nothing really special.

* It a simple program that initially calls `configure_led()` to initialize the RGB LED driver.
* Then in a infinite loop it periodically calls `blink_led()` to turn the RGB LED on (with a hardcoded color) and off.

Sorry, I didn't talk about that in class. Maybe you don't need it, but let me write it down.

* `led_strip_t` is a `typedef`-ed struct whose fields are all function pointers. (We saw something about function pointers in [2-basics/3-function_pointer/](../../2-basics/3-function_pointer/)). This is a typical way to implement something similar to `abstract class` in higher level languages (e.g. Java, TypeScript) and thus to achieve polymorphism in C. The [led_strip_init](./0/main/blink_example_main.c#L44) function returns a pointer to an instance of `led_strip_t` that contains function pointers that point to functions that represent *one possible implementation* of the interface.

Let's see a similar (but more reduced) example:

```c
struct my_interface {
    void (*method_a)(int foo);
    int (*method_b)(void);
};
typedef struct my_interface my_interface;

void method_a_impl_x(int foo) {
    // ...
}
int method_b_impl_x(void) {
    // ...
}
void init_x(my_interface *i) {
    i->method_a = method_a_impl_x;
    i->method_b = method_a_impl_x;
}

void method_a_impl_y(int foo) {
    // ...
}
int method_b_impl_y(void) {
    // ...
}
void init_y(my_interface *i) {
    i->method_a = method_a_impl_y;
    i->method_b = method_a_impl_y;
}

void do_something_with_my_interface(my_interface *i) {
    i->method_a(1);
    i->method_b();
}

int main() {
    my_interface x;
    init_x(&x);

    my_interface y;
    init_y(&y);

    do_something_with_my_interface(&x);
    do_something_with_my_interface(&y);
}
```

There are many little variants on how polymorphism can be implemented in C, but they are rely on the use of function pointers. As you can see, it's a lot less succinct, elegant and automagic than higher level languages. It ain't much, but it's honest work.

## 1

As we already saw in [2-modularity](../../2-basics/2-modularity), we know that in C we can use `scanf()` to get user input. We also know that ESP-IDF (i.e. the SDK) already provides implementation of `printf()` and `scanf()` that under the hood use the UART to send the output and to get the input. Finally we also know that our board ([ESP32-C3-DevKitM-1](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/hw-reference/esp32c3/user-guide-devkitm-1.html)) is equipped with an UART-to-USB converter, so by connecting via USB the board to our PC we can communicate with the SoC ESP32-C3-MINI-1, on which our firmware runs.

**CHANGELOG**: 

* Add code to make scanf work "as expect". Unfortunately, it still doesn't echo back the characters that we insert. This is a limitation of the implementation of ESP-IDF.
* Blink LED using the insert RGB intensities.

## 2

So far, quite easy. But now we want that the LED starts blinking immediately and keeps blinking. When the user finishes to insert the RGB values, the blinking LED shall change its color to match the inserted RGB values.

The fundamental problem is the `scanf()` blocks the execution until the input is available. So how can we achieve our goal? Well "easy", since `scanf()` blocks the execution, how about having multiple *threads* of execution?

**CHANGELOG**: 

* Introduce FreeRTOS. Create multiple *threads* of execution (a.k.a. tasks in FreeRTOS) using [`xTaskCreate`](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/system/freertos.html#_CPPv411xTaskCreate14TaskFunction_tPCKcK8uint32_tPCv11UBaseType_tPC12TaskHandle_t).


