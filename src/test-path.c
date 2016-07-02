#include "path.c"
#include <stdio.h>
#include <assert.h>

#define PATH_MAX 4096

typedef struct {
  const char** args;
  const char* result;
} test_vector_t;

static test_vector_t tests[] = (test_vector_t[]){
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

static const char* dirname_tests[] = (const char*[]){
  "a/b", "a",
  "/a/b", "/a",
  "a/b/", "a/",
  "/a/b/", "/a/",
  "a", "",
  "a/", "",
  "/a", "/",
  "/a/", "/",
  "", "",
  "/", "/",
  0
};

static const char* ext_tests[] = (const char*[]){
  "a.b", "b",
  "a.b/", "b",
  "a.b/c", "",
  "a.b/c.d", "d",
  "/", "",
  "a/", "",
  "a.b.c", "c",
  "", "",
  0
};

static const char* base_tests[] = (const char*[]){
  "a.b", "a.b",
  "a.b/", "a.b",
  "a.b/c", "c",
  "a.b/c.d", "c.d",
  "/", "",
  "a/", "a",
  "a.b.c", "a.b.c",
  "", "",
  0
};

int main() {
  char store[PATH_MAX];
  test_vector_t *test = &(tests[0]);
  while (test->args) {
    const char** args = test->args;
    const char* expected = test->result;

    printf("join");
    while (*args) {
      printf(" '\033[32m%s\033[0m'", *args);
      args++;
    }
    printf(" = '\033[32m%s\033[0m'\n", expected);
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
      printf("\033[1;31mFailed\033[0m: Expected '\033[32m%s\033[0m', but got: '\033[32m%.*s\033[0m'\n", expected, buffer.len, buffer.data);
      assert(matched);
    }
    test++;
  }

  const char **dirtest = dirname_tests;
  while (*dirtest) {
    const char* dir = *dirtest++;
    const char* expected = *dirtest++;
    printf("dirname '\033[32m%s\033[0m' = '\033[32m%s\033[0m'\n", dir, expected);
    path_t result = path_dirname(path_cstr(dir));
    int matched = strlen(expected) == result.len && strncmp(expected, result.data, result.len) == 0;
    if (matched) {
      printf("\033[34mSuccess\033[0m\n");
    }
    else {
      printf("\033[1;31mFailed\033[0m: Expected '\033[32m%s\033[0m', but got: '\033[32m%.*s\033[0m'\n", expected, result.len, result.data);
      assert(matched);
    }
  }

  const char **exttest = ext_tests;
  while (*exttest) {
    const char* path = *exttest++;
    const char* expected = *exttest++;
    printf("extension '\033[32m%s\033[0m' = '\033[32m%s\033[0m'\n", path, expected);
    path_t result = path_extension(path_cstr(path));
    int matched = strlen(expected) == result.len && strncmp(expected, result.data, result.len) == 0;
    if (matched) {
      printf("\033[34mSuccess\033[0m\n");
    }
    else {
      printf("\033[1;31mFailed\033[0m: Expected '\033[32m%s\033[0m', but got: '\033[32m%.*s\033[0m'\n", expected, result.len, result.data);
      assert(matched);
    }
  }

  const char **basetest = base_tests;
  while (*basetest) {
    const char* path = *basetest++;
    const char* expected = *basetest++;
    printf("basename '\033[32m%s\033[0m' = '\033[32m%s\033[0m'\n", path, expected);
    path_t result = path_basename(path_cstr(path));
    int matched = strlen(expected) == result.len && strncmp(expected, result.data, result.len) == 0;
    if (matched) {
      printf("\033[34mSuccess\033[0m\n");
    }
    else {
      printf("\033[1;31mFailed\033[0m: Expected '\033[32m%s\033[0m', but got: '\033[32m%.*s\033[0m'\n", expected, result.len, result.data);
      assert(matched);
    }
  }

}
