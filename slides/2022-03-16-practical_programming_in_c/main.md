---
title: Practical Programming in C
date: March 16, 2022
---

## Why C?

* More portable
    * Writing portable C code is not trivial.
    * But for virtually any target architecture, there will be a C compiler for it.
* Less complex
    * Abstraction is double-edged sword.
    * "*C makes it easy to shoot yourself in the foot; C++ makes it harder, but when you do it blows your whole leg off.*" - Bjarne Stroustrup
    * Even if used C++, need to cut many things: exception, STL, templates, etc.
        * Often end up with "C with classes".
* Alternatives: C++, Rust, Micropython, Nim, Lua, etc.

## Goals and non-goals

* Time is limited. Based also on the survey, some trade-off had to be made.
* Goal: talk about peculiarities of C, with a strong focus on what I think you'll need for the project.
* Non-goal: introduce to programming. Control flow statements, concept of data types, concept of function, common arithmetic operators, etc.

## Let's look at the hello world.

* [CMakeLists](https://github.com/chenlijun99/unibz-76088A-materials/blob/c07026b1bf29bff9bcb07653d893ab20bb4ec507/1-hello_world/CMakeLists.txt)
    * It's not vanilla CMake. See the [documentation of ESP-IDF build system](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html#).
    * You should not need it, but in the same page they also explain [interoperability with pure CMake](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html#writing-pure-cmake-components)
* [hello_world_main.c](https://github.com/chenlijun99/unibz-76088A-materials/blob/c07026b1bf29bff9bcb07653d893ab20bb4ec507/1-hello_world/main/hello_world_main.c)
    * What we see: use of `#include`; function and function calls; the `stdio.h` standard library; `typedef` and `struct`; dereference operator and pointers; bitwise, arithmetic and ternary operator; for loop; FreeRTOS's delay function.

# A quick introduction to pointers [@kernighanProgrammingLanguageANSI1988, chapter 5]

## It's quite simple actually...

* (Simple does not always mean easy)
* The data storage (memory) available to a C program is one or more contiguous sequences of bytes. Each byte in memory has a unique address [@MemoryModelCppreference].
* A pointer is a variable that contains the address of a variable
* Address of a variable of type `t` has type `t*`.
* To get the address of a variable use the unary operator `&`.
* To access the object the pointer points to use the unary operator `*` (called also *indirection* or *dereference* operator)

## Example I

* Let's take 1 minute to parse the following snippet

    ```c
    int x = 1, y = 2, z[10];
    int *ip; 
    ip = &x; 
    y = *ip; 
    *ip = 0; 
    ip = &z[0]; 
    ```

## Example II

* Do the comments match with your expectations?

    ```c
    int x = 1, y = 2, z[10];
    int *ip; /* ip is a pointer to int */
    ip = &x; /* ip now points to x */
    y = *ip; /* y is now 1 */
    *ip = 0; /* x is now 0 */
    ip = &z[0]; /* ip now points to z[0] */
    ```

## Pointers are everywhere

* If you familiar with Java, Python, JavaScript, etc. you should have heard of:
    * *shallow copy vs deep copy*
    * *pass-by-reference vs pass-by-value*. 
* When shallow copy and pass-by-reference are concerned, under the hood there are pointers!
* Pointers are a mean to perform direct memory manipulation. This is empowering (and endangering).
    *  I can take a arbitrary address, "treat it as" the address of an `int` and write to it.

        ```c
        *((int*)123) = 1;
        ```

* High-level languages (Java, C#, JavaScript, Python, etc.) don't give you direct access to the memory, to protect (and limit) you.

## Pass-by-reference in C

* In C we can only pass-by-value...
* ...but by passing pointer values and dereferencing the pointer parameters in the function, we can simulate pass-by-reference

    ```c
    void swap(int *px, int *py) {
        int temp;
        temp = *px;
        *px = *py;
        *py = temp;
    }
    ```

* This technique is also used to have multiple output variables from a function. [`scanf(const char *format, ...)`](https://en.cppreference.com/w/c/io/fscanf) is a perfect example of such use.

# Crash course on modular C and CMake

## Before starting...

* All the code materials is at [this GitHub repository](https://github.com/chenlijun99/unibz-76088A-materials)
* Prerequisite tools
    * Compiler: `GCC` or `Clang`
    * Build system: `Make` or `Ninja`
* Peculiar terminologies
    * *Object* [@wg14/n1256C99StandardDraft2007, section 3.14]
        * Simply a region of memory that contains data
        * Not an instance of a class. Nor the root of the class hierarchy (e.g. Java)
    * *Identifier* [@wg14/n1256C99StandardDraft2007, section 6.2.1; section 6.4.2.1]
        * An identifier is a sequence of case-sensitive nondigit characters and digits, which designates objects, functions, etc.
        * More simply: the name of some entity.
    * The C standard does not define the term *variable*, but we can think of it as an identifier that refers to an object.

## How does C compilation work? {.allowframebreaks}

![C++ compilation process [Prog](https://commons.wikimedia.org/wiki/User:Prog), [C++ compilation process](https://commons.wikimedia.org/wiki/File:C++_compilation_process.svg), [CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/legalcode)
](assets/C++_compilation_process.svg){height=80%}

* The concept of *translation unit*: the unit of program text after preprocessing [@wg14/n1256C99StandardDraft2007, section 5.1.1.1].
* The term *build* is commonly used to refer to the whole process (preprocessing, compilation, linking).
* [1-hello_world](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/1-hello_world): Compile a C program with a single *translation unit* 
    * Compilation is quite complex and the compiler itself invokes quite a few programs (preprocessor, linker) under the hood.
    * We need to understand where the error comes from.

## How to achieve modularity {.allowframebreaks}

* Let's look at example [2-modularity](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity). It will be a dense journey.
* We mainly went to know about:
    * [Linkage of identifiers](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/1-base/README.md#3).
    * [Storage duration](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/1-base/README.md#4) of objects and their lifetime.
    * [The difference between declaration and definition](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/1-base/README.md#4).
        * The one definition rule.
    * [Header files](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/2-header_file/README.md#1).
    * [`struct`](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/2-header_file/README.md#2).
    * [Header guards](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/2-header_file/README.md#3).
    * Conditional compilation:
        * [Using #if, #ifdef, etc.](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/2-header_file/README.md#4)
        * [Coarse conditional compilation using selective compilation](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/2-header_file/README.md#5).
    * [Risks of using the underlying binary representation of data. Use serialization/deserialization](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/2-header_file/README.md#4).
    * [Add additional include file search paths](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/2-header_file/README.md#6).
    * [Use of CMake](https://github.com/chenlijun99/unibz-76088A-materials/tree/main/2-basics/2-modularity/3-cmake/README.md).

### Rule of thumb for modular C

* Put the public interface of a module in the module's header file
    * Use *header guards*
* The public interface typically consists of declaration (not definition) of functions, declaration or definition of other user-defined types (i.e. `enum`, `struct`, `union`), `typedef`s, and pubic constants.
    * While it is discouraged, if you really need a global variable to be part of the public interface, declare it in the header file by using the `extern` *storage-class specifier* and **not** initializing it.
* All the public function and variable names must be unique throughout the whole program. Establish a naming convention!
* Put the implementation details in source files.
* The private functions and global variables must be placed in the module's source file(s) and they must be declared using the `static` *storage-class specifier*, to ensure they have *internal linkage*.
