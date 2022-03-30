# Function pointers

A simple C program to show the use of function pointers.

**Takeaways**:

* Syntax to use for a "function pointer type".
    * E.g. using `void (*op)(int current, size_t index, int *arr, size_t arr_len);` we are declaring a variable called `op` that is a pointer to a function that returns void and has `int current, size_t index, int *arr, size_t arr_len` as its parameters.
    * `typedef` can be used to create alias to custom types. They are often used to create function pointer types, since they are quite verbose.
    * Things can become complex. E.g. what if we want to declare an array of function pointers that return in their turn function pointers? I hope you won't need that and I don't think you need to remember that syntax. As always, Google is your friend.
        * Also, using `typedef` can help you break down the complex syntax.
* Minors things:
    * [`sizeof` operator](https://en.cppreference.com/w/c/language/sizeof)
    * [`#define` with parameters](https://en.cppreference.com/w/c/preprocessor/replace)
    * VLA (Variable Length Array) in function `inline_map`.
        * Introduced in C99
        * VLAs are discouraged in Embedded Programming, as they can make it impossible to determine the amount of stack that the firmware may use.
