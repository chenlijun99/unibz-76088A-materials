# Step: base

Goal: from a rough sketch of modular C code that doesn't compile, arrive to a program that compiles and works as expected.

## 0

**Initial considerations and takeaways:**

* Concerning the `send` function:
    * Arrays decay into pointers. Typically the size of the array is passed along.
    * The `printf` is a [variadic function](https://en.cppreference.com/w/c/variadic). To my knowledge variadic functions always determine the variadic part of the arguments using the previous fixed arguments (e.g. `ioctl` is another example of such variadic function). In the case of printf-like functions, the fixed parameter is the format string. Further information on the format specifiers can be found at https://en.cppreference.com/w/cpp/io/c/fprintf.
        * In general it is important to make sure that the type of the argument and the type of the format specifier match. That's why we cast `buffer[i]` to `unsigned int`. We see a case of [type casting](https://en.cppreference.com/w/c/language/cast) in C (a.k.a. type conversion, type coercion, etc.).
            * Detail: actually in this case the cast is not necessary due to the default argument promotion of the variadic arguments. See C99, Paragraph 6.5.2.2.7.
    * Relationship between array and pointers and the equivalence of the "subscript operator" with "pointer arithmetic + dereference". E.g. `a[i]` is equivalence to `*(arr + i)`.
* Concerning the `get_current_temperature` function:
    * An example of the use of the `scanf()` function to obtain input from the user.
* Historically C did not have the boolean data type. `stdbool.h` was added in C99.
* Use of fixed width integer types from `stdint.h` (since C99) for better portability. See more on [cppreference](https://en.cppreference.com/w/c/types/integer).

Let's try to build the whole project

```sh
$ gcc main.c spi_bus.c temperature_sensor.c
# Result: compiler error: `error_number` undeclared
```

## 1

**CHANGELOG:** To solve the compilation error of the previous step, we added the declaration `bool is_in_error;` in main.

Let's try to build the whole project

```sh
$ gcc main.c spi_bus.c temperature_sensor.c
# Result: linker error: multiple definitions
```

The result is a linker error, however we also get some warnings. Actually C99 forbids use of implicit declaration of functions.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c -pedantic-errors -std=c99
# Result: compiler error: implicit declaration of functions
```

**Takeaways:**

* Prior to C99, functions could be used undeclared. The compilers would assume its type to be `int f()`, meaning that anything can be passed to it and it returns an `int`.
     * This behaviour has been removed in C99, but GCC issues only warnings rather than errors unless we use `-pedantic-errors`.
     * Therefore, a empty parameter list in C does not mean that the function accepts no parameter. If means that the function accepts any parameter. To express that the function accepts no parameter (so that the compiler gives an error if you are erroneously passing a parameter), put `void` in parameter list.

## 2

**CHANGELOG:** To solve the compilation error of the previous step, we added the declaration of the functions used in the `main` function in `main.c`. Furthermore, we put `void` in the parameter list of functions that take no arguments.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c -pedantic-errors -std=c99
# Result: linker error: multiple definitions
```

Just the same as:

```sh
$ gcc main.c spi_bus.c temperature_sensor.c
# Result: linker error: multiple definitions
```

## 3

**CHANGELOG:** To solve the linker error of the previous step, we added prefixes to the functions and we put `static` to the private functions.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c
# Result: builds!
```

**Takeaways:**

* *Linkages of identifiers*. See C99, Section 6.2.2.
    * Linkage:
        > An identifier *declared* in different scopes or in the same scope more than once can be made to refer to the same object or function by a process called linkage. There are three kinds of linkage: external, internal, and none.
    * File scope identifiers for functions and objects, can have *internal linkage* or *external linkage*.
        * Identifiers with *internal linkage* refer to the same object or function *within one translation unit*.
            * From an API design point of view, anything that is *private* in a translation unit should have internal linkage.
        * Identifiers with *external linkage* refer to the same object or function *in the whole program*.
            * From an API design point of view, anything that is *public* in a translation unit should have external linkage.
            * Naming convention (e.g. using prefix) is the only solution to avoid conflicts of identifiers with external linkage
        * *One definition rule*: 
            * An identifier declared with internal linkage can be defined at most once in the translation unit. See C99, Paragraph 6.9.3.
            * An identifier declared with external linkage can be defined at most once in the whole program. See C99, Paragraph 6.9.5.
        * By default file scope identifiers have external linkage.
        * We can use the `extern` *storage-class specifier* to make explicit that an identifier shall have external linkage.
        * We can use the `static` storage-class specifier to declare that an identifier shall have internal linkage.
    * A block scope identifier may have external linkage using the `extern` storage-class specifier.
    * There are also identifier that have *no linkage*. See C99, Paragraph 6.2.2.6.

## 4

Tried to execute the compiled program, but it doesn't do anything... 

**CHANGELOG:** Oh. We need to initialize the `is_idle` to true.

When we didn't initialize `is_idle`, what was its value?

**Takeaways:**

* Concept of *storage duration* of objects. See C99, Section 6.2.4.
    * Objects with *static storage duration*
        * Their lifetime spans the entire execution of the program.
        * They are initialized exactly once, prior to program startup.
            * Prior to program startup... Mhh, so who does this? The C *language runtime library*. See [wikipedia](https://en.wikipedia.org/wiki/Runtime_library) for what is a runtime library. Some examples of components of the runtime library of a language:
                * Software floating-point calculation functions used by the compiler when the hardware architecture does not have a FPU.
                * Garbage collector
        * **An uninitialized object with static storage duration is *zero-initialized*. See C99, Paragraph 6.7.8.10.**
        * In practice they are variables with file scope (colloquially known as *global variables*) and variables declared using the `static` storage-class qualifier inside functions.
            * Remember that we previously shown that we can use `static` to declare that an identifier shall have internal linkage? In C the meaning of `static` is overloaded: you use it on an identifier at file-scope, to specify its linkage; you use on an identifier that denotes an object (i.e. a variable) at block-scope, to specify its storage duration.
                * Actually one more meaning was introduced in C99. See C99, Paragraph 6.7.5.3 if you are curious.
    * Objects with *automatic storage duration*:
        * Their lifetime spans the block in which they are declared.
        * The value of an uninitialized object with automatic storage duration is indeterminate. Typically such objects are allocated in the *stack* and whatever is in the stack in that moment constitutes their initial value.
            * Why doesn't the C language define a known initial value? Because initializing an object has overhead, while the philosophy of C is: trust the programmers and don't make them pay for what they might not use.
        * They are the so called *local variables*.
    * Objects with *allocated storage duration*:
        * Objects that reside in the so called *dynamic memory* or *heap*.
        * Their lifetime starts upon an allocation (e.g. via `malloc()`) and ends upon deallocation (via `free()`).
            * The higher level language equivalent of `malloc()` is the `new` keyword (e.g. `new MyAwesomeClass()`). For those of you who are familiar with Python and JavaScript, I think when you create a list or dictionary (Python), object or array (JavaScript), they are typically allocated in the heap.
            * In languages with a garbage collector (Java, Python, JavaScript, etc.) there is no need to explicitly `free()` an object allocated in heap. When the object is no more referenced by any variable, it is freed by the garbage collector.
        * They are typically frown upon in embedded programming as they introduce *memory fragmentation* and thus non-determinism in memory availability.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c
# Result: linker error: multiple definition of `is_idle';
```

**CHANGELOG:** Okay... Well we have to put `static` on `is_idle`.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c
# Result: builds!
```

But why didn't the linker previously complain about the multiple definition of `is_idle`? They have the same name and have external linkage. Why does it complain now?

**Takeaways:**

* The difference between *declaration* and *definition*. See C99, Paragraph 6.7.5.
    * *A declaration specifies the interpretation and attributes of a set of identifiers.*. In other words a declaration tells the compiler: I'm using this identifier to refer to this kind of entity (an object with certain type, a function with certain signature, etc.), which may be "created" here or elsewhere.
    * A definition is a special type of declaration that:
        * for an object, causes storage to be reserved for that object;
        * for a function, includes the function body.
* Indeed in step 2 and 3 we had the problem of multiple definitions because we were *defining* the functions. We didn't have problems of multiple definitions with `is_idle` and `error_number` because we only *declared* them.

**CHANGELOG:** Therefore, also the `error_number` variable is being shared in all translation units and we don't have multiple definition problems just because we're not defining it. We want the `error_number` of each module to be public, so we must add a prefix to the identifier, as learnt in step 3. So we get `spi_bus_error_number` and `temperature_sensor_error_number`. While we know that we don't need to initialize them to zero, since C zero-initializes them for us, we still explicitly initialize them with zero in their respective translation unit. In order to access them in `main.c` we still have to declare (not define) them.
Let's try to build again.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c
# Result: builds!
```

As expected, it builds!

BUT WAIT!

If the standard defines a definition of a object as: "causes storage to be reserved for that object", then the fact that in step 3 the program successfully built and we were reading from `is_idle` means that there was some storage reserved for it (the same discourse applies to `error_number`). So we not only declared `is_idle`. We did define `is_idle`. Actually, we have not seen yet what exactly means defining an object (i.e. defining a variable). We only know that the result of the definition is that some storage will be reserved for the object.

Look at: C99, Section 6.9.2.

**Takeaways:**

* Declaration + initialization => Definition
* Declaration + no initialization + (no storage-class specifier or `static`) => *Tentative definition*
    * Why it exists, historical reasons. You can read this [stack overflow answer](https://stackoverflow.com/a/33294644) if interested.

So, what we had in step 3 was to have multiple *tentative definitions* of `is_idle`. But why didn't the linker give us an error then? Well, it's common for compilers to generate code in such a way that the linker is able to resolve all the tentative definitions present in multiple translation units into a single object. The C99 standard also lists this as a common extension (see C99, Section J.5.11). To disable this compiler extension we can pass the `-fno-common` flag.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c -fno-common
# Result: linker error: multiple definition of `spi_bus_error_number', multiple definition of `temperature_sensor_error_number'.
```

**CHANGELOG**: To avoid making tentative definitions, we add the `extern` storage-class specifier to the declarations of `temperature_sensor_error_number` and `spi_bus_error_number` in `main.c`.

**Takeaways:**

* To declare a variable and not define it we:
    * must use the `extern` storage-class specifier, so that the declaration is not a tentative definition;
    * must not initialize the variable, so that the declaration is not a definition.
