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

## 2.1 detour

One of your classmates asked something along the lines of: "What if multiple threads use `scanf()` at the same time?".

* I didn't know the answer.
    * Maybe `scanf()` must by contract be thread-safe?
        * As C99 is concerned I don't think so, because C99 has no notion of concurrency. Support for concurrency was introduced in C11.
    * Maybe the implementation of `scanf()` provided by ESP-IDF internally makes use of mutex (or any other locking primitives)?
* So during the lesson we decided to just try it.

**CHANGELOG**:

* Create two threads, running the `input_task` function.
* Move the UART config functions out from `input_task`, so that they are executed only once.
* Allocate two strings, `name1` and `name2`, each passed to one thread (using the 4th parameter of the `xTaskCreate` function).

**Takeaways**:

* One function can be executed in multithread threads. We can think that a function can have multiple instances. Just like a program is a passive element lying in your disk and a process is a program in execution (with all its resources), a function is a passive element whose corresponding machine code lies in the memory. A thread can be thought as a function in execution (with all its resources, most notably its own stack). Just like we can have multiple instances of our browser open, we can have multiple threads that run our `input_task` function.
    * Obviously, sharing stuff among processes is less direct, while threads share the same address space (in other words, can access the same global variables). So it is really easy to unknowingly access some shared data and incur in race conditions.

If you build and flash you would see that the name of the threads are not being correctly printed. Why? Because we allocated the two strings locally, and so they have automatic storage duration, which means that their lifetime ends after exiting the block in which they were defined (i.e. the `app_main` function). OTOH, we are passing them to two threads, whose execution time is decided by the scheduler of FreeRTOS and we don't have control. What's happening is that the execution of `app_main` is finished and `name1` and `name2` are deallocated, then FreeRTOS schedules the two threads to run, but when we access `name1` and `name2` passed as parameters, they are no more valid, thus the quirky (undefined) behaviour.

**Takeaways**:

* Pay attention to storage duration of variables!

You could try to move the definition of `name1` and `name2` out from `app_main`, i.e. in the global scope, and try to rebuild and flash. It should work.
You could also try to put a infinite loop at the end of `app_main`, so that `app_main` never finishes and `name1` and `name2` are never deallocated. It should work.

Now it is working. Even if there is some interleaving of `printf`, we can see that we can properly insert inputs using `scanf()`. Indeed, if we tracked down the call stack, `scanf()` internally calls [`uart_read`](https://github.com/espressif/esp-idf/blob/8153bfe4125e6a608abccf1561fd10285016c90a/components/vfs/vfs_uart.c#L242-L275) and we can see that the UART operations are enclosed with `_lock_acquire_recursive` and `_lock_release_recursive`. So the shared UART resource is being protected using locks. That's why it works.

## 3

Problem: we want the change of RGB color to be picked only after all the three base colors are inserted. Furthermore, we want to avoid any kind of race conditions deriving from concurrently modifying shared data. We need to put access to the colors in `g_task_shared` in a critical section.

**CHANGELOG**:

* Use mutex to protect access to `g_task_shared`.

**Takeaways**:

* Use of the [mutex API](https://www.freertos.org/Real-time-embedded-RTOS-mutexes.html) of FreeRTOS to achieve mutual exclusion
when accessing thread shared data.
