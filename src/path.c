#include "path.h"
#include <string.h>
#include <stdio.h>

path_t path_cstr(const char* str) {
  path_t result;
  result.data = str;
  result.len = strlen(str);
  return result;
}

int path_add(mpath_t *base, path_t path) {
  // Do nothing if both are empty.
  if (base->len == 0 && path.len == 0) return 0;

  // If the base is empty and the path is absolute, preserve leading slash
  if (base->len == 0 && base->max > 0 && path.len > 0 && path.data[0] == '/') {
    base->data[0] = '/';
    base->len = 1;
  }

  // Remove any trailing slash in base if there is path
  if (path.len > 0 && base->len > 1 && base->data[base->len - 1] == '/') {
    base->len--;
  }

  // If the base is exactly '.', truncate it.
  if (base->len == 1 && base->data[0] == '.') {
    base->len = 0;
  }

  unsigned int i = 0;
  for (;;) {
    // Skip any leading slashes in the path
    while (i < path.len && path.data[i] == '/') i++;

    // Find the next slash (or end of string);
    unsigned j = i;
    while (j < path.len && path.data[j] != '/') j++;

    // Match end of segment
    if (j == i) break;

    path_t segment = {
      .data = path.data + i,
      .len = j - i
    };

    // Skip '.' segments
    if (segment.len == 1 && segment.data[0] == '.') {
      i = j;
      continue;
    }

    // Match '..' segment
    if (j - i == 2 && path.data[i] == '.' && path.data[i + 1] == '.') {
      // If this is an empty path, preserve the '..'
      if (base->len == 0) {
        if (base->max < 2) return -1;
        base->data[0] = '.';
        base->data[1] = '.';
        base->len = 2;
        i = j;
        continue;
      }

      // If the path is just a slash, leave it alone.
      if (base->len == 1 && base->data[0] == '/') {
        i = j;
        continue;
      }

      // Remove trailing slash, if there is one.
      if (base->len > 1 && base->data[base->len - 1] == '/') { base->len--; }

      // If the path ends in '..' already, add another.
      if (base->len >= 2 && base->data[base->len - 1] == '.' && base->data[base->len - 2] == '.' && (
          base->len == 2 || (base->len > 2 && base->data[base->len - 3] == '/'))) {
        if (base->len + 3 >= base->max) return -1;
        base->data[base->len++] = '/';
        base->data[base->len++] = '.';
        base->data[base->len++] = '.';
        i = j;
        continue;
      }

      // Pop one segment, including slash.
      while (base->len && base->data[base->len - 1] != '/') base->len--;
      // Preserve leading slashes.
      if (base->len == 0 && base->data[0] == '/') base->len = 1;
      i = j;
      continue;
    }

    // If the base is empty and the segment is absolute, insert leading slash
    // Or if the base does not end in slash, add one as separator
    if (base->len ?
        base->data[base->len - 1] != '/' :
        (path.len && path.data[0] == '/')) {
      if (base->len + 1 >= base->max) return -1;
      base->data[base->len++] = '/';
    }

    // Ensure space in the buffer
    if (base->len + segment.len >= base->max) return -1;

    memcpy(base->data + base->len, segment.data, segment.len);
    base->len += segment.len;

    i = j;
  }

  // Use '.' if the base ends up empty.
  if (base->len == 0 && base->max > 0) {
    base->data[0] = '.';
    base->len = 1;
  }

  // Make sure ending matches path
  if (path.len) {
    if (path.data[path.len - 1] == '/') {
      if (base->data[base->len - 1] != '/') base->data[base->len++] = '/';
    }
    else {
      if (base->len > 1 && base->data[base->len - 1] == '/') base->len--;
    }
  }
  return 0;
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
