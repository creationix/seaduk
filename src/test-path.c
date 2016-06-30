#include "path.c"
#include <stdio.h>
#include <assert.h>

#define PATH_MAX 4096

typedef struct {
  const char** args;
  const char* result;
} test_vector_t;

test_vector_t tests[] = (test_vector_t[]){
  {(const char*[]){".", "x/b", "..", "/b/c.js", NULL}, "x/b/c.js"},
  {(const char*[]){"/.", "x/b", "..", "/b/c.js", NULL}, "/x/b/c.js"},
  {(const char*[]){"/foo", "../../../bar", NULL}, "/bar"},
  {(const char*[]){"foo", "../../../bar", NULL}, "../../bar"},
  {(const char*[]){"foo/", "../../../bar", NULL}, "../../bar"},
  {(const char*[]){"foo/x", "../../../bar", NULL}, "../bar"},
  {(const char*[]){"foo/x", "./bar", NULL}, "foo/x/bar"},
  {(const char*[]){"foo/x/", "./bar", NULL}, "foo/x/bar"},
  {(const char*[]){"foo/x/", ".", "bar", NULL}, "foo/x/bar"},
  {(const char*[]){"./", NULL}, "./"},
  {(const char*[]){".", "./", NULL}, "./"},
  {(const char*[]){".", ".", ".", NULL}, "."},
  {(const char*[]){".", "./", ".", NULL}, "."},
  {(const char*[]){".", "/./", ".", NULL}, "."},
  {(const char*[]){".", "/////./", ".", NULL}, "."},
  {(const char*[]){".", NULL}, "."},
  {(const char*[]){"", ".", NULL}, "."},
  {(const char*[]){"", "foo", NULL}, "foo"},
  {(const char*[]){"foo", "/bar", NULL}, "foo/bar"},
  {(const char*[]){"", "/foo", NULL}, "/foo"},
  {(const char*[]){"", "", "/foo", NULL}, "/foo"},
  {(const char*[]){"", "", "foo", NULL}, "foo"},
  {(const char*[]){"foo", "", NULL}, "foo"},
  {(const char*[]){"foo/", "", NULL}, "foo/"},
  {(const char*[]){"foo", "", "/bar", NULL}, "foo/bar"},
  {(const char*[]){"./", "..", "/foo", NULL}, "../foo"},
  {(const char*[]){"./", "..", "..", "/foo", NULL}, "../../foo"},
  {(const char*[]){".", "..", "..", "/foo", NULL}, "../../foo"},
  {(const char*[]){"", "..", "..", "/foo", NULL}, "../../foo"},
  {(const char*[]){"/", NULL}, "/"},
  {(const char*[]){"/", ".", NULL}, "/"},
  {(const char*[]){"/", "..", NULL}, "/"},
  {(const char*[]){"/", "..", "..", NULL}, "/"},
  {(const char*[]){"", NULL}, "."},
  {(const char*[]){"", "", NULL}, "."},
  {(const char*[]){" /foo", NULL}, " /foo"},
  {(const char*[]){" ", "foo", NULL}, " /foo"},
  {(const char*[]){" ", ".", NULL}, " "},
  {(const char*[]){" ", "/", NULL}, " /"},
  {(const char*[]){" ", "", NULL}, " "},
  {(const char*[]){"/", "foo", NULL}, "/foo"},
  {(const char*[]){"/", "/foo", NULL}, "/foo"},
  {(const char*[]){"/", "//foo", NULL}, "/foo"},
  {(const char*[]){"/", "", "/foo", NULL}, "/foo"},
  {(const char*[]){"", "/", "foo", NULL}, "/foo"},
  {(const char*[]){"", "/", "/foo", NULL}, "/foo"},
  {NULL,NULL},
};

int main() {
  char store[PATH_MAX];
  test_vector_t *test = &(tests[0]);
  while (test->args) {
    const char** args = test->args;
    const char* expected = test->result;

    int comma = 0;
    while (*args) {
      if (comma) printf(" ");
      comma = 1;
      printf("'\033[32m%s\033[0m'", *args);
      args++;
    }
    printf(" = '\033[32m%s\033[0m'?\n", expected);
    args = test->args;

    mpath_t buffer = (mpath_t){
      .data = store,
      .len = 0,
      .max = PATH_MAX
    };
    while (*args) {
      path_t path = path_cstr(*args);
      path_add(&buffer, path);
      args++;
    }
    if (buffer.len == 0) {
      buffer.len = 1;
      buffer.data[0] = '.';
    }

    int matched = strlen(expected) == buffer.len && strncmp(expected, buffer.data, buffer.len) == 0;
    if (matched) {
      printf("\033[34mSuccess\033[0m\n");
    }
    else {
      printf("\033[1;31mFailed\033[0m: Expected \033[32m%s\033[0m, but got: \033[32m%.*s\033[0m\n", expected, buffer.len, buffer.data);
      assert(matched);
    }
    test++;
  }
}
