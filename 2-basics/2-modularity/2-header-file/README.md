# Step: use header files

Goal: from the result of the [previous step](../1-base/), arrive at the typically C modularity approach.

## 0

Problems:

* Imagine if other than `main.c`, the functions from the `spi_bus` and `temperature_sensor` module need to be used in many other source files. The current setup means that we would have to re-declare their public functions and variables in every source file where we use them. What if at some point the signature of some functions from these two modules need to be changed? We have to change all the declarations that we added in each source file. **The code is not DRY (Don't Repeat Yourself).**.

## 1

**CHANGELOG:** To make the code more DRY, we put all the declarations of the public functions and variables in a *header file*.

Let's try to build the whole project

```sh
$ gcc main.c spi_bus.c temperature_sensor.c
# Result: builds successfully!
```

**Takeaways:**

* In C there is no concept of module (a proper module system has been introduced in C++ 20).
* By using variables and functions with external linkage, we implicitly make them accessible from other translation units.
* It is customary to use header files (which typically have the extension `.h`) and source files (which typically have the extension `.c`) to implement a module.
    * From a conceptual point of view:
        * Header files are files where the public interface of a module lies.
        * Source files are files where the implementation of a module lies.
    * Note that the `#include` preprocessor directive merely looks for the file and does a copy paste. The `.h` extension is just a convention. Following it may be good for your IDE, editor and other tooling to work well, but for the preprocessor any file works.
* Difference between `#include ""` and `#include <>`. The first search for the header file first relative to the current directory, i.e. the directory of the file that contains the `#include` directive. If it cannot find the header file, the it falls back to the same behaviour of the second, which searches the header file in implementation defined paths (e.g. on Linux `gcc` looks also in `/usr/include/`). 

## 2

Now let's say we want to add a configuration to the initialization function of our temperature_sensor.

**CHANGELOG:** Added a public configuration data structure to the `temperature_sensor` module. Passed an instance of the configuration to the in `main.c`.

Let's build.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c
# Result: builds successfully!
```

**Takeaways:**

* Defining user-defined data type using `struct`.
    * As with variables and functions, we put public data structures of a module in the header file.
    * Use of the `.` member access operator to access the field of a `struct`. The same syntax can been seen in many other languages such as Java, JavaScript, Python, etc. to access the member of an object.
* Passing const pointers as a way to avoid copy: C passes everything by value, but our `struct` may become quite big in future. We don't want to make an useless copy. 
* Use of the conditional ternary operator.

## 3

Now let's say we want to add a configuration module that takes care of loading the application configuration from NVM (non-volatile memory). It gathers the configurations from other modules into a single `struct`.

**CHANGELOG:** Added configuration module (the pair `configuration.c/.h` files)

**Takeaways:**

* Use of the `->` operator as a shorthand for dereference + member access. E.g. `(*foo).bar` is equivalent to `foo->bar`.

Let's build.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c configuration.c
# Result: compilation error: redefinition of ‘struct temperature_sensor_config’
# ...
```

Note that this time the redefinition error is from the compiler, not the linker.

**CHANGELOG:** Added header guards

Let's build again.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c configuration.c
# Result: builds successfully!
```

**Takeaways:**

* The `struct` is a purely C language-level construct that has no "reflection" in the machine code. The linker, whose job is "filling the holes" in the machine code, doesn't deal with `struct`s.
	* Indeed C doesn't support [reflection](https://en.wikipedia.org/wiki/Reflective_programming).
    * So the one definition rule is not something that is diagnosed only by the linker. Other constructs of the C language are also subject to the one definition rule (e.g. `enum`, `typedef`, etc.) and are diagnosed by the compiler.
* Some constructs that are typically put in a header file are repeatable (e.g. declaration of functions), but some other constructs should appear only once (e.g. definition of a `struct`). To ensure that a header file is included only once in a translation unit, we have to use header guards.
    * A header guard is a case of [conditional compilation](https://en.cppreference.com/w/c/preprocessor/conditional) using preprocessor directives.
    * A more succinct and commonly supported alternative to header guards is `#pragma once`, but it is not standard-compliant.

## 4

As for now the implementation of `configuration_load_from_nvm` fakes values from a NVM by setting some hard-coded magic values.
There are other ways we could fake a NVM, e.g. using a file, or asking for user input.

Let's say we want to provide different implementations of the `configuration_load_from_nvm` function. 

**CHANGELOG:** Added different implementations of the `configuration_load_from_nvm` function, which can be "configured" at compile time using the macros `CONFIGURATION_FAKE_NVM_VIA_USER_INPUT`, `CONFIGURATION_FAKE_NVM_VIA_FILE` and `CONFIGURATION_FAKE_NVM_DEFAULT_USE_FAHRENHEIT`.

Let's build again.

```sh
$ gcc -DCONFIGURATION_FAKE_NVM_VIA_USER_INPUT main.c spi_bus.c temperature_sensor.c configuration.c
# Result: builds successfully!
# Execution result: the configuration of use_fahrenheit is asked to the user

$ gcc -DCONFIGURATION_FAKE_NVM_VIA_FILE main.c spi_bus.c temperature_sensor.c configuration.c
# Result: builds successfully!
# Execution result: the configuration of use_fahrenheit is read from the file `fake_nvm`. If the file does not exist it is created and filled with a default value, which may depend on CONFIGURATION_FAKE_NVM_DEFAULT_USE_FAHRENHEIT. We can also manually change the values in the file and see what happens.

$ gcc -DCONFIGURATION_FAKE_NVM_DEFAULT_USE_FAHRENHEIT=1 main.c spi_bus.c temperature_sensor.c configuration.c
# Result: builds successfully!
# Execution result: the configuration of use_fahrenheit is set to true

$ gcc -DCONFIGURATION_FAKE_NVM_DEFAULT_USE_FAHRENHEIT=0 main.c spi_bus.c temperature_sensor.c configuration.c
# Result: builds successfully!
# Execution result: the configuration of use_fahrenheit is set to false
```

**Takeaways:**

* Having different implementations of the same function is common in cross-platform programming
    * Often in embedded programming, we *do* cross-platform programming because we want the business-logic of our firmware to run on the host (i.e. the development machine, e.g. our laptop) so that the hardware-independent parts can be tested on the host.
    * We'll discuss more systematically how to test embedded systems.
* More on conditional compilation
    * Use of the `#if`, `#ifdef`, etc. directives for conditional compilation. See [cppreference](https://en.cppreference.com/w/c/preprocessor/conditional).
    * To define macros using command line, `gcc` supports the `-DMACRO_NAME[=<MACRO_VALUE>]` option. You can define an arbitrary number of macros by passing this option multiple times.
    * Conditional compilation is useful to:
        * Reduce final executable size.
            * While, it is true that nowadays the compiler can perform some clever dead code elimination and the linker is able to drop functions and variables that are not used by anybody, there is still some limit in what they can do. Similar to how tree-shaking in modern web development.  
            * With conditional compilation preprocessor directives we have fine grained control.
        * Especially on cross-platform programming, sometimes we must use conditional compilation because the functions that we use may not be available in other platforms. E.g.

        ```c
        #include <linux_apis.h>
        #include <windows_apis.h>

        void do_something_platform_specific(bool is_linux) {
            if (is_linux) {
                linux_api_do_something();
            } else {
                windows_api_do_something();
            }
        }
        ```

        The code above is often impossible to compile! If we compile for Linux we don't have `windows_apis.h` and viceversa.

        So we must use conditional compilation

        ```c
        #ifdef USE_LINUX
        #include <linux_apis.h>
        #else
        #include <windows_apis.h>
        #endif

        void do_something_platform_specific() {
        #ifdef USE_LINUX
            linux_api_do_something();
        #else
            windows_api_do_something();
        #endif
        }
        ```

* Use of `assert()` from `assert.h` to verify conditions that must be true
    * Note: our example is actually not the most appropriate use case for `assert()`.
* We see again an example of "treating" a variable like an byte array and just manipulate it. Is it really "okay" to just treat arbitrary objects as byte array? In other words, is it okay to directly work with the underlying binary representation of the data? Typically the answer is no. It's not portable. At least the following aspects are not universally "fixed". They may depend on architecture, optimization level, etc.
    * [Signed number representations](https://en.wikipedia.org/wiki/Signed_number_representations)
    * [Data structure alignment](https://en.wikipedia.org/wiki/Data_structure_alignment)
    * [Endianness](https://en.wikipedia.org/wiki/Endianness)
    * The problem is that nobody has agreed on these things.
    * We need to perform serialization and deserialization: agree on the data format and convert the program data to such data format as it (i.e. the program data) leaves the program, e.g. for transmission, for storage, etc..
    * Examples of serialization and deserialization solutions.
        * Java `Serializable`.
        * JSON, XML.
        * Protocol Buffer.
        * FlatBuffer.

## 5

**CHANGELOG:**: separated configuration.c into configuration_common.c configuration_fake_default.c, configuration_fake_file.c, and configuration_fake_user_input.c.

```sh
$ gcc main.c spi_bus.c temperature_sensor.c configuration_common.c configuration_fake_user_input.c
# Result: builds successfully!
# Execution result: the configuration of use_fahrenheit is asked to the user

$ gcc main.c spi_bus.c temperature_sensor.c configuration_common.c configuration_fake_file.c
# Result: builds successfully!
# Execution result: the configuration of use_fahrenheit is read from the file `fake_nvm`. If the file does not exist it is created and filled with a default value, which may depend on CONFIGURATION_FAKE_NVM_DEFAULT_USE_FAHRENHEIT. We can also manually change the values in the file and see what happens.
```

**Takeaways:**

* A coarse-grained conditional compilation based on "choosing which file to compile" can be performed to reduce conditional compilation complexity.
* There is no need to have 1-1 correspondence between header file and source file.

## 6

Now, with so many files, it's becoming messy. I want to put each module in its own folder.

**CHANGELOG:**: simply create a folder for each module and copied all its files in the folder.

Let's try to build

```sh
$ gcc main.c spi_bus/spi_bus.c temperature_sensor/temperature_sensor.c configuration/common.c configuration/fake_user_input.c
# Result: preprocessor error: no such file or directory
# I.e. cannot find haeder files.

$ gcc main.c -Ispi_bus/ -Itemperature_sensor -Iconfiguration spi_bus/spi_bus.c temperature_sensor/temperature_sensor.c configuration/common.c configuration/fake_user_input.c
# Result: builds successfully!
```

**Takeaways:**

* With `gcc` we can use the `-I` option to add additional search paths for the `#include` directive
