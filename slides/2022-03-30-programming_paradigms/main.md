---
title: Programming models for Embedded systems
date: March 30, 2022
---

# A refresher on operating systems [@tanenbaumModernOperatingSystems2015, section 2.3; @silberschatzOperatingSystemConcepts2018, chapter 6] and a quick look at FreeRTOS.

## I want to do this... *and* this..., *at the same time*

* In our example ([3-blink/2-freertos](https://github.com/chenlijun99/unibz-76088A-materials/blob/dev/3-blink/2-freertos/README.md)), we wanted to: wait for input *and* blink the LED *at the same time*.
* We used the function [`xTaskCreate`](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/system/freertos.html#_CPPv411xTaskCreate14TaskFunction_tPCKcK8uint32_tPCv11UBaseType_tPC12TaskHandle_t) to create multiple threads of execution.
    * Note: the term *thread* is more commonly used, while FreeRTOS uses to term *task*. We'll use them interchangeably.
* We checked [the datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf). The ESP32C3 has 1 core, so why can we have multiple threads of execution?

## Context switch {.allowframebreaks}

* Context switch: The process of saving the context of the thread being suspended and restoring the context of the thread being resumed.
* What is the *context*? CPU registers + Stack.
    * PC (Program Counter): where I am
    * SP (Stack pointer): how I arrived here (call stack) and my local data
    * etc.
* The stack is already in RAM, but the CPU registers need to be explicitly dumped into RAM during the context switch.
* Who decides which thread to resume? An essential component of an OS: the *scheduler*.
* Overhead of context switch: CPU registers saving and loading + scheduler decision + ...

![Illusion of contemporaneity by context switching among threads. Source: [FreeRTOS documentation](https://www.freertos.org/implementation/a00004.html)](assets/concurrency.png)

## Concurrency vs Parallelism

* Concurrency (a.k.a. Multitasking): being able to deal with multiple things at the same time
    * Opposite of sequential execution.
* Parallelism: effectively doing multiple at the same time

## Preemption

* When does a context switch happen?
    * When a thread yields control voluntarily. Typically when OS primitives such as delay ([`vTaskDelay`](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/system/freertos.html#_CPPv410vTaskDelayK10TickType_t)), acquire mutex, wait for semaphore (e.g. [`xSemaphoreTake`](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/system/freertos.html#c.xSemaphoreTake)), etc. are called.
    * Via preemption
* Preemption: the execution of a task is interrupted without cooperation from task itself.
    * How can it happen?

## Interrupts

* "An interrupt is a request for the processor to interrupt currently executing code, so that the event can be processed in a timely manner." - adapted from [Wikipedia](https://en.wikipedia.org/wiki/Interrupt)
* Many types of interrupts: peripheral, the software, a fault (e.g. dereferencing a NULL pointer, dividing by 0).
* Interrupt vector table: we can think it as an array of function pointers, which are called ISR (*Interrupt Service Routine*).
* How it works:
    1. An interrupt request happens, for whatever reason
    2. The processor saves where it was (similar to context switch, but performed by the CPU)
    3. Based on the source and type of the interrupt the CPU determines which callback function to call .
* So an interrupt basically performs preemption. Interrupts are a preemption mechanism built-in the CPU. Interrupts are the root of preemption!
    * FreeRTOS performs preemption via an periodic tick interrupt. [See more here](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/freertos-smp.html#tick-interrupts).

## Example of program using an interrupt

* [3-blink/2-interrupt](https://github.com/chenlijun99/unibz-76088A-materials/tree/dev/3-blink/2-interrupt)

## The `volatile` type qualifier [@regehrNineWaysBreak2010; @WhyVolatileType]

* Forces the compiler to perform memory access. Essentially: disables optimization.
* Used for memory-mapped I/O
    * E.g. [gpio_get_level](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/peripherals/gpio.html#_CPPv414gpio_get_level10gpio_num_t) -> [gpio_ll_get_level](https://github.com/espressif/esp-idf/blob/c9646ff0beffc86d2c6d1bfbad34da16e328e0e3/components/hal/esp32s3/include/hal/gpio_ll.h#L282-L289) -> [gpio_dev_t](https://github.com/espressif/esp-idf/blob/c9646ff0beffc86d2c6d1bfbad34da16e328e0e3/components/soc/esp32c3/include/soc/gpio_struct.h#L21)
* Can be used to ensure that memory writes are visible to other concurrent executions. But do not rely on `volatile` for synchronization in concurrency.
    * Instruction reordering may happen
        * I prepare some data and then I set a flag. What if due to instruction reordering, the flag is set first? Memory barriers are necessary.
    * Non-atomicity (e.g. what if I modify a `volatile uint64_t` but the CPU has no instruction to access 64-bit values?)

## Race conditions

* Interrupt can fire at any time (e.g. somebody can send me an UART byte at any time).
* In turn the OS can preempt the thread at any time.
* It causes non-deterministic interleaving of execution, which may lead to race conditions.
* A race condition arises in software when a computer program, to operate properly, depends on the sequence or timing of code that executes concurrently (e.g. multiple threads, interrupts).

```{=latex}
\end{frame}
\begin{frame}[fragile]{Example 1 - race conditions in tasks}
    Assume: a = 0

    \vspace{1cm}
    \begin{columns}
        \begin{column}{0.45\textwidth}
          \begin{minted}[autogobble, fontsize=\scriptsize, frame=single, escapeinside=||]{c}
            // task 1
            int tmp = a; |\only<2>{\textcolor{red}{1}}| |\only<3>{\textcolor{red}{1}}|
            tmp = tmp + 1; |\only<2>{\textcolor{red}{2}}| |\only<3>{\textcolor{red}{2}}|
            a = tmp; |\only<2>{\textcolor{red}{3}}| |\only<3>{\textcolor{red}{6}}|
          \end{minted}
        \end{column}
        \begin{column}{0.45\textwidth}
          \begin{minted}[autogobble, fontsize=\scriptsize, frame=single, escapeinside=||]{c}
            // task 2
            int tmp = a; |\only<2>{\textcolor{red}{4}}| |\only<3>{\textcolor{red}{3}}|
            tmp = tmp + 1; |\only<2>{\textcolor{red}{5}}| |\only<3>{\textcolor{red}{4}}|
            a = tmp; |\only<2>{\textcolor{red}{6}}| |\only<3>{\textcolor{red}{5}}|
          \end{minted}
        \end{column}
    \end{columns}
    \vspace{1cm}

    \only<2>{Output: a = 2}
    \only<3>{Output: a = 1}
\end{frame}
```

```{=latex}
\begin{frame}[fragile]{Example 2 - race conditions with interrupt}
    \begin{columns}
        \begin{column}{0.45\textwidth}
          \begin{minted}[autogobble, fontsize=\scriptsize, frame=single, escapeinside=||]{c}
            // task
            while (1) {
                if (!interrupt_handled) { |\only<2>{\textcolor{red}{1}}|
                    interrupt_handled = true; |\only<2>{\textcolor{red}{3}}|
                }
            }
          \end{minted}
        \end{column}
        \begin{column}{0.45\textwidth}
          \begin{minted}[autogobble, fontsize=\scriptsize, frame=single, escapeinside=||]{c}
            // interrupt
            interrupt_handled = false; |\only<2>{\textcolor{red}{2}}|
          \end{minted}
        \end{column}
    \end{columns}
    \vspace{1cm}

    \only<2>{Result: lost one interrupt}
\end{frame}
```

## Conclusion

* Interrupt can fire at any time, preemption can happen any time.
* Threads' execution can interleave non-deterministically.
* Threads communicate by sharing memory.
* Shared mutable memory + non deterministic execution interleaving => race conditions.

## Critical sections

* "The only way to solve concurrency-related problems is via localized sequential execution" - I don't know who said this. 
    * Preemption at arbitrary location is problematic. We solve this by disabling preemption locally, in some places, whenever shared resources are modified. These places are called *critical sections*.
* Critical sections:
    * In general, any piece of concurrent code that accesses shared resources is a critical section and should be protected.
    * Often used to refer to a specific way to protect a critical section by disabling interrupts.
        * E.g. vanilla FreeRTOS provides [taskENTER_CRITICAL() and taskEXIT_CRITICAL()](https://www.freertos.org/taskENTER_CRITICAL_taskEXIT_CRITICAL.html) for this.
* *Locking* is used to protect critical sections.
    * Many types,
    * may reside at different levels of abstraction,
    * but ultimately locks must be used somewhere to deal with preemptive concurrent execution.

## Disable interrupts

* Should be as short as possible, to keep the system responsive.

    ```c
    disable_interrupt();
    // Long running computation
    // In the meanwhile, interrupts arrive but are not
    // serviced. Quite bad.
    enable_interrupt();
    ```
* Not possible on multicore systems.
    * E.g. ESP32 (not ESP32-C3) is a dual-core SoC. For this reason ESP-IDF actually provides a modified version of FreeRTOS in which entering a critical section requires the use of a spin lock. See [here](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/freertos-smp.html#critical-sections).

## Atomic primitives

* Primitives with perform two memory operations atomically.
* E.g. test and set: atomically changes the state of a flag to true and returns the value it held before. 

    ```c
    // this code must execute atomically
    bool test_and_set(bool *target) {
        bool rv = *target;
        *target = true;
        return rv;
    }
    ```
* Typically used to implement higher-level synchronization primitives.

::: notes
The two race examples we saw previously are all due to the fact that we perform at least two memory operations (specifically first read and then write), but a preemption may happen in the middle. If, with some mechanism (e.g. disabling interrupt or hardware support), we can ensure the atomicity of these two memory operations, then we have an atomic primitive, which is the basis of many other higher-level multithreaded synchronization primitives.
:::

## Spin lock {.fragile}

* Spin locks: busy waiting using atomic primitives

    \begin{minted}[autogobble, fontsize=\scriptsize, escapeinside=||, frame=single]{c}
    // flag shared among threads
    bool lock = false;
    \end{minted}

    \begin{minted}[autogobble, fontsize=\scriptsize, escapeinside=||, frame=single, breaklines, highlightlines={3,5}]{c}
    // body of a task
    do {
        while (test_and_set(&lock)); /* |{\bf take lock}|, or |{\bf busy wait}| until lock is available */
        /* critical section */
        lock = false; /* |{\bf release lock}| */
        /* remainder (non critical) section */
    } while (true);
    \end{minted}

## Mutex {.fragile}

* Mutex: mutual exclusion via blocking, rather than busy waiting

    * E.g. [FreeRTOS's Mutex API](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/system/freertos.html#c.xSemaphoreCreateMutex).
    * E.g. [3-blink/2-freertos, step 3](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/3-blink/3-freertos/3)
* Pseudocode of simplistic implementation
    \vspace{\baselineskip}
    \begin{columns}
        \begin{column}{0.45\textwidth}
          \begin{minted}[autogobble, fontsize=\scriptsize, frame=single, breaklines, escapeinside=||]{c}
          void mutex_lock(bool mutex) {
              while (test_and_set(&mutex)) {
                block(); // the thread yields control, rather than busying waiting
              }
          }
          \end{minted}
        \end{column}
        \begin{column}{0.45\textwidth}
          \begin{minted}[autogobble, fontsize=\scriptsize, frame=single, breaklines, escapeinside=||]{c}
          void mutex_unlock(bool mutex) {
            mutex = false;
            unblock_others(); // wake up the blocked threads
          }
          \end{minted}
        \end{column}
    \end{columns}

### Busy waiting vs blocking

* Busy waiting: I'm stay here and wait until it's ready.
* Blocking: OS, wake me up when what I need is available.
    * Typically, blocking is preferred as it doesn't waste CPU cycles uselessly.

## What makes an RTOS special?

* All what we have seen apply on operating systems in general. What makes an RTOS special?
* Can be used to implement *real-time* systems. Unlike general purpose OSes (e.g. Windows, Linux, macOS).
    * I.e. system with *real-time* requirements.
        * E.g. The brake has to be activated within 10 milliseconds.
* Deterministic deadline-oriented scheduling
* Bounded overhead
* What an RTOS doesn't do for you:
    * Assign priorities correctly
    * Bound the length of critical sections
    * Bound the resource hold time
