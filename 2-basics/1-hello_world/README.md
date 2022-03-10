# 1-hello_world

A really simple C program

## Commands we used

```sh
# Preprocess 1-hello_world.c
$ gcc -E 1-hello_world.c
# Alternatively. `cpp` is the preprocessor
$ cpp 1-hello_world.c

# Preprocess without line directives
$ gcc -P -E 1-hello_world.c
# Alternatively 
$ cpp -P 1-hello_world.c

# Compile 1-hello_world.c to generate an object file, but don't link
$ gcc -c 1-hello_world.c
# Link the 1-hello_world.o from the previous command to generate a C program
$ gcc 1-hello_world.o

# Compile and link 1-hello_world.c to generate a C program
$ gcc 1-hello_world.c

# See how complex stuff happens under the hood. We don't need to understand them.
$ gcc -v 1-hello_world.c
```

## What we also saw

### A invocation of the compiler may produce errors from different sources

* Preprocessor level error
    * Try to change `#include <stdio.h>` to `#include <stdio.>`.
* Compilation level error
    * Try to remove a semicolon.
* Linker level error
    * Try to remove the main function
    * Or try to declare a function and not define it.
