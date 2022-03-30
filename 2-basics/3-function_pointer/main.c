/**
 * Toy example to illustrate the use of function pointers.
 */
#include <stdio.h>
#include <string.h>

#define COUNT_OF(arr) (sizeof(arr) / sizeof(arr[0]))

static void for_each(int *arr, size_t len,
                     void (*op)(int current, size_t index, int *arr,
                                size_t arr_len)) {
  for (size_t i = 0; i < len; ++i) {
    op(arr[i], i, arr, len);
  }
}

typedef int (*map_op)(int current, size_t index, int *arr, size_t arr_len);

static void inline_map(int *arr, size_t len, map_op op) {
  int copy[len];
  memcpy(copy, arr, sizeof(copy));
  for (size_t i = 0; i < len; ++i) {
    arr[i] = op(copy[i], i, copy, len);
  }
}

static void print(int current, size_t index, int *arr, size_t arr_len) {
  printf("%d%s", current, index < arr_len - 1 ? ", " : "\n");
}

static int add1(int current, size_t index, int *arr, size_t arr_len) {
  return current + 1;
}

int main() {
  int arr[5] = {1, 2, 3, 4, 5};

  for_each(arr, sizeof(arr) / sizeof(arr[0]), print);

  inline_map(arr, COUNT_OF(arr), add1);

  for_each(arr, COUNT_OF(arr), print);
}
