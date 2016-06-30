#ifndef PATH_H
#define PATH_H

#include <stdbool.h>

typedef struct {
  const char *data;
  unsigned int len;
} path_t;

typedef struct {
  char *data;
  unsigned int len;
  unsigned int max;
} mpath_t;

union {
  path_t path;
  mpath_t mpath;
} either_t;


path_t path_cstr(const char* str);
bool path_eq(path_t a, path_t b);
bool path_add(mpath_t *base, path_t path);
path_t path_dirname(path_t path);
path_t path_extension(path_t path);
path_t path_basename(path_t path);

#endif
