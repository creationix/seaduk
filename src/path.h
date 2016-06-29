#ifndef PATH_H
#define PATH_H

typedef struct {
  const char *data;
  int len;
} path_t;

typedef struct {
  char *data;
  int len;
  int max;
} mpath_t;

union {
  path_t path;
  mpath_t mpath;
} either_t;


path_t path_cstr(const char* str);
void path_add(mpath_t *base, path_t path);
path_t path_dirname(path_t path);
path_t path_extension(path_t path);
path_t path_filename(path_t path);

#endif
