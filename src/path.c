#include "path.h"
#include <string.h>


path_t path_cstr(const char* str) {
  path_t result;
  result.data = str;
  result.len = strlen(str);
  return result;
}

void path_add(mpath_t *base, path_t path) {
  if (base->len > 0) {
    base->data[base->len++] = '/';
  }
  enum {
    EMPTY,
    START,
    NORMAL,
    ONEDOT,
    TWODOT,
  } state = base->len ? START : EMPTY;
  for (int j = 0; j < path.len && base->len < base->max; j++) {
    char next = path.data[j];
    switch (state) {
      case START:
        if (next == '/') {
          continue;
        }
      case EMPTY:
        if (next == '.') {
          state = ONEDOT;
        }
        else {
          state = NORMAL;
        }
        break;
      break;
      case NORMAL:
        if (next == '/') state = START;
      break;
      case ONEDOT:
        if (next == '.') state = TWODOT;
        else if (next == '/') {
          state = START;
          base->len -= 2;
        }
        else state = NORMAL;
      break;
      case TWODOT:
        if (next == '/') {
          base->len -= 3;
          while (base->len > 0 && base->data[--base->len] != '/');

          state = base->len > 0 ? START : EMPTY;
          printf("base='%.*s' base.len=%d\n", base->len, base->data, base->len);
        }
      break;
    }
    base->data[base->len++] = next;
  }
}

path_t path_dirname(path_t path) {
  path_t result;
  result.data = path.data;
  result.len = path.len;
  for (int i = 0; i < path.len; i++) {
    if (path.data[i] == '/') {
      result.len = i;
    }
  }
  return result;
}

path_t path_extension(path_t path) {

}

path_t path_filename(path_t path) {

}
