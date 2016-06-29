#include "path.c"
#include <stdio.h>
#include <assert.h>

#define PATH_MAX 4096

void test(const char* left, const char* right, const char* expected) {
  char store[PATH_MAX];
  strncpy(store, left, PATH_MAX);
  mpath_t buffer = (mpath_t){
    .data = store,
    .len = strlen(left),
    .max = PATH_MAX
  };
  path_add(&buffer, path_cstr(right));
  printf("'%s' + '%s' = '%s'? '%.*s'\n", left, right, expected, buffer.len, buffer.data);
  assert(strlen(expected) == buffer.len);
}

int main() {
  test("Hello", "World", "Hello/World");
  test("Hello", "/./World", "Hello/World");
  test("Hello", "//World", "Hello/World");
  test("Hello/World", "../Galaxy", "Hello/Galaxy");
  test("/Hello", "../Goodbye//World", "/Goodbye/World");
  test("Hello", "../Goodbye//World", "Goodbye/World");
  test("", "/absolute", "/absolute");
  test("is", "/relative", "is/relative");
}
