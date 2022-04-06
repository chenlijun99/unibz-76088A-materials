# Step: Interrupt

Goal: from the base project, we want to toggle the blinking behaviour of our firmware by using a button.

## 0

Same as [2-freertos step 0](../2-freertos/README.md#0)

## 1

**CHANGELOG**: 

* Conditionally include sdkconfig.release, in which we override the default Kconfig and specifically we enable compiler optimization.
* In [`bz_gpio_init()`](./1/main/blink_example_main.c#L64-85), use GPIO APIs to register an interrupt handler that toggle a boolean variable on and off.

Now we can build and flash our firmware.

```sh
# Build and flash using the "normal configuration"
$ idf.py -B build set-target "esp32c3"
$ idf.py flash monitor
# Test it! It should work.

# Build and flash having compiler optimizations turned on
$ idf.py -DCMAKE_BUILD_TYPE="Release" -B build-release set-target "esp32c3"
$ idf.py -B build-release flash monitor
# Test it! It should still work.

# Note that while it works, you should see some red warning messages. It's the watchdog!

# Now remove the volatile qualifier at line 58 in blink_example_main.c.
# And rebuild and flash.
$ idf.py -B build-release flash monitor
# Test it! You should see that the program doesn't work. Clicking on the button doesn't have any effect! Why? Because of compiler optimizations!
```

**Takeaways**: 

* Concrete example of interrupts.
* [GPIO APIs from ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/peripherals/gpio.html).
* Use of the `volatile` type qualifier to suppress optimizations, which may introduce unexpected behaviour to our program.
    * In our case `volatile` was necessary because otherwise from the compiler point of view there is no way in [this snippet](./1/main/blink_example_main.c#L93-L102) the variable the variable `button_toggle_status` could become true: it is initialized to `false` and the [loop condition](./1/main/blink_example_main.c#L95) means that it will never enter the loop, since `button_toggle_status` is `false`. The compiler doesn't taken into account the fact the `gpio_isr_handler` interrupt handler could be called at any machine without being explicitly invoked in C code. Therefore, the compiler could completely removed the [inner while loop](./1/main/blink_example_main.c#L95-L101), since it will never be entered (from its point of view).
* Watchdog:
    * In general a watchdog is a hardware module in the SoC that must be periodically "kicked", i.e. refreshed. If the refresh timeout expires, the watchdog typically resets the system (you can see it as a reboot). Watchdog is used as a last-resort escape hatch that restarts the system when for some reason the firmware is hung somewhere and doesn't "kick" it. Note that computers are still physical objects and can fail in very subtle ways. E.g. cosmic rays could flip a bit in your CPU registers/in your RAM and lead the execution of your program to an unknown place.
    * What we saw in this example is a watchdog that yells at us because our code is never yielding execution to other tasks, since we wait for `button_toggle_status` to change in a tight loop. We busy wait, rather than block on a OS primitive. Our code is monopolizing the CPU, causing starvation of other tasks.
    * See also [ESP-IDF documentation on watchdog](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/system/wdts.html).

## 2

Problem: we know that, even with `volatile`, using `button_toggle_status` as synchronization mechanism between the interrupt and the task is not the correct thing to do. Furthermore, we're doing busy waiting and, as we have seen, this makes the watchdog angry. We have also learnt that semaphores are often used for synchronization.

**CHANGELOG**: 

* Substitute the shared `button_toggle_status` with a semaphore.

**Takeaways**: 

* Use of [binary semaphore API](https://www.freertos.org/Embedded-RTOS-Binary-Semaphores.html) of FreeRTOS to synchronize between interrupt handler and task.
