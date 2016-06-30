#include "path.c"
#include <stdio.h>
#include <assert.h>

#define PATH_MAX 4096

void test(const char* left, const char* right, const char* expected, int resolve) {
  char store[PATH_MAX];
  mpath_t buffer = (mpath_t){
    .data = store,
    .len = 0,
    .max = PATH_MAX
  };
  if (!resolve || *right != '/') {
    path_add(&buffer, path_cstr(left));
  }
  path_add(&buffer, path_cstr(right));
  printf("'%s' + '%s' = '%s' ? '%.*s'\n", left, right, expected, buffer.len, buffer.data);
  assert(strlen(expected) == buffer.len);
  assert(strncmp(expected, buffer.data, buffer.len) == 0);
}

int main() {
  test("", "a", "a", 0);
  test("", "/a", "/a", 0);
  test("", "a/", "a/", 0);
  test("", "/a/", "/a/", 0);

  test("a", "b", "a/b", 0);
  test("a/", "b", "a/b", 0);
  test("a", "b/", "a/b/", 0);
  test("a", "/b/", "a/b/", 0);
  test("/a", "b", "/a/b", 0);
  test("/a/", "b", "/a/b", 0);
  test("/a", "b/", "/a/b/", 0);
  test("/a", "/b/", "/a/b/", 0);

  test("a///", "b", "a/b", 0);
  test("a", "///b", "a/b", 0);
  test("a", "b///", "a/b/", 0);
  test("///a", "b", "/a/b", 0);

  test("a/./", "", "a/", 0);
  test("a/.", "", "a", 0);
  test("", "./a", "a", 0);
  test("", "/./a", "/a", 0);

  test("", ".", ".", 0);
  test("", "./", "./", 0);

  test("", "..", "..", 0);
  test("", "../", "../", 0);
  test("", "/..", "/", 0);
  test("", "/../", "/", 0);

  test("", "../b", "b", 0);
  test("", "/../b", "/b", 0);
  test("a", "../b", "b", 0);
  test("a", "/../b", "b", 0);
  test("/a", "../b", "/b", 0);
  test("/a", "/../b", "/b", 0);

  // test("Hello", "/./World", "Hello/World", 0);
  // test("Hello", "//World", "Hello/World", 0);
  // test("Hello/World", "../Galaxy", "Hello/Galaxy", 0);
  // test("/Hello", "../Goodbye//World", "/Goodbye/World", 0);
  // test("Hello", "../Goodbye//World", "Goodbye/World", 0);
  // test("", "/absolute", "/absolute", 0);
  // test("is", "/relative", "is/relative", 0);
  //
  // test("Hello", "World", "Hello/World", 1);
  // test("Hello", "/./World", "/World", 1);
  // test("Hello", "//World", "/World", 1);
  // test("Hello/World", "../Galaxy", "Hello/Galaxy", 1);
  // test("/Hello", "../Goodbye//World", "/Goodbye/World", 1);
  // test("Hello", "../Goodbye//World", "Goodbye/World", 1);
  // test("", "/absolute", "/absolute", 1);
  // test("is", "/relative", "/relative", 1);
}
