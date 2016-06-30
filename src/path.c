#include "path.h"
#include <string.h>


path_t path_cstr(const char* str) {
  path_t result;
  result.data = str;
  result.len = strlen(str);
  return result;
}

typedef enum {
  EMPTY,
  START,
  NORMAL,
  ONEDOT,
  TWODOT,
} path_parse_state_t;


static void path_pop(mpath_t *base) {
  // Ignore empty paths
  if (!base->len) return;
  
}

void path_add(mpath_t *base, path_t path) {
  // If the new segment is empty, ignore it.
  if (!path.len) return;

  // If the base is empty and the segment is absolute, insert leading slash
  // Or if the base does not end in slash, add one as separator
  if (base->len ? base->data[base->len - 1] != '/' : path.data[0] == '/') {
    base->data[base->len++] = '/';
  }

  // Run the new segment through the state machine.
  path_parse_state_t state = START;
  for (unsigned int i = 0; i < path.len && base->len < base->max; i++) {
    char next = path.data[i];
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
          base->len -= 1;
          continue;
        }
        else state = NORMAL;
      break;
      case TWODOT:
        if (next == '/') {
          if (base->len >= 3) {
            base->len -= 3;
            while (base->len && base->data[--base->len] != '/');
            state = START;
          }
          else {
            base->len = 0;
          }
          continue;
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
