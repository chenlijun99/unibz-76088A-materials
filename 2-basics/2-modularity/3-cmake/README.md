# Step: use CMake

Goal: from the result of the [previous step](../2-header-file/), arrive at a setup where we perform automatic and incremental build using CMake

## 0

Problems:

* I don't want to re-build all the translation units whenever I make even the tiniest change.

We know that by passing the `-c` flag, we tell the compiler to compile the translation units, but not try to link them into an executable. So we can do this.

```sh
# Compile the source files, but don't link
$ gcc -c main.c -Ispi_bus/ -Itemperature_sensor -Iconfiguration spi_bus/spi_bus.c temperature_sensor/temperature_sensor.c configuration/common.c configuration/fake_user_input.c
# Link the compiled object files
$ gcc main.o spi_bus.o temperature_sensor.o common.o fake_user_input.o

# If I change a source file, I just need to re-compile the changed source file
# E.g. I change configuration/common.c
$ gcc -c -Itemperature_sensor configuration/common.c
# And then link everything again.
$ gcc main.o spi_bus.o temperature_sensor.o common.o fake_user_input.o
```

But what if a header file is changed? Then I would need to re-compile all the source files that include that header file. The compiler is able to provide the header file dependency information (e.g. on `gcc` using the `-MM` and `-MT` options), so with enough shell scripting we can achieve the goal of "re-compiling what changed", but why reinventing the wheel? 

## 1

**CHANGELOG:** To make building an application easy, we use CMake!


```sh
$ mkdir build
$ cd build
$ cmake -DCONFIGURATION_FAKE_IMPL="fake_user_input" ..
$ make 

# Use a different build system, ninja
$ cmake -G Ninja ..
$ ninja
```

**Takeaways:**

* As always [awesome-*](https://github.com/onqtam/awesome-cmake#resources) style repositories are quite awesome. You can find a lot of resources. Sorry, I didn't check the correctness of the linked materials, but if there is any contradiction between what I show and what you read, please tell me. It is an occasion for me to learn.
* Look at the comments in [CMakeLists.txt](./1/CMakeLists.txt). 
* CMake uses an out-of-source build approach, i.e. all the build artifacts are kept in a separate directory instead of alongside the source files. Advantages:
    * Multiple build directories with different build configuration
        * We can use the [CMAKE_BUILD_TYPE](https://cmake.org/cmake/help/v3.5/variable/CMAKE_BUILD_TYPE.html] cache variable to control the type of the build. Most notably
            * `Debug` ensures that the code is compiled with debug symbols, which allow us to map the machine code back to the source code, making code stepping, breakpoint placement, etc., easier (similar to source maps in JavaScript).
            * `Release` compiles with all the optimizations turned on.
    * Easier to work with version control. Typically build artifacts are not kept into version control (the basic idea is that anything that can be *generated* should not be put into version control). By having build artifacts in separate directories, it is trivial to just ignore those directories (e.g. add the directories in `.gitignore` if we use `git`).
* For any project, at least one top-level `CMakeLists.txt` is needed. In the top-level `CMakeLists.txt` the commands `cmake_minimum_required` and `project` must be used.
* Cache variables as persisted across CMake executions and typically serve as customization points for our build. 
    * We can pass the option `-D<VARIABLE_NAME>=<VARIABLE_VALUE>` to CMake to force setting a cache variable. We are basically changing some configuration of our build.
        * Note that while the syntax is very similar to the option `-DMACRO[=<MACRO_VALUE>]` of `gcc`, they are *completely different things, that operate on different levels, but that serve the same purpose: conditional build*.

## 2

Problem: as more and more modules are added to the application, having a single top-level CMakeLists.txt makes things unmaintainable. Just like we want to modularize the C code, we want to modularize also the CMake code.

**CHANGELOG:** More modular CMake. Create one CMakeLists.txt for each module and create one static library target for each module.

**Takeaways:**

* Look at the comments in [CMakeLists.txt](./2/CMakeLists.txt) and [CMakeLists.txt](./2/configuration/CMakeLists.txt).
* Static libraries as a way to group a set of object files.
* Usage requirements and how their transitive propagation can be controlled using `PUBLIC/PRIVATE/INTERFACE` and `target_link_libraries`. See more [here](https://cmake.org/cmake/help/v3.5/manual/cmake-buildsystem.7.html#transitive-usage-requirements) .
    * Usage requirement: what should be passed to the preprocessor/compiler/linker for the correct build of a target (correct build of all the source files associated to it).
        * include directory to add to the compiler's (or better, preprocessor's) search paths: using [target_include_directories](https://cmake.org/cmake/help/v3.5/command/target_include_directories.html)
        * macros to be defined on command line: using [target_compile_definitions](https://cmake.org/cmake/help/v3.5/command/target_compile_definitions.html),
        * other targets to link: using [target_link_libraries](https://cmake.org/cmake/help/v3.5/command/target_link_libraries.html)
        * other compiler flags: using [target_compile_options](https://cmake.org/cmake/help/v3.5/command/target_compile_options.html)
    * PRIVATE usage requirement: only I (the target) have this usage requirements.
    * PUBLIC usage requirement: both I (the target) and any target to which I link have this usage requirements.
    * INTERFACE usage requirement: I don't have this usage requirement but any target to which I link have this usage requirements.
