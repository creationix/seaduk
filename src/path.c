#include "path.h"
#include <string.h>
#include <stdio.h>

path_t path_cstr(const char* str) {
  path_t result;
  result.data = str;
  result.len = strlen(str);
  return result;
}

bool path_eq(path_t a, path_t b) {
  if (a.len != b.len) return false;
  for (unsigned int i = 0; i < a.len; i++) {
    if (a.data[i] != b.data[i]) return false;
  }
  return true;
}


bool path_add(mpath_t *base, path_t path) {
  // Do nothing if both are empty.
  if (base->len == 0 && path.len == 0) return true;

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
        if (base->max < 2) return false;
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
        if (base->len + 3 >= base->max) return false;
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
      if (base->len + 1 >= base->max) return false;
      base->data[base->len++] = '/';
    }

    // Ensure space in the buffer
    if (base->len + segment.len >= base->max) return false;

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
  return true;
}

path_t path_dirname(path_t path) {
  int absolute = path.len && path.data[0] == '/';
  int trailing = path.len > 1 && path.data[path.len - 1] == '/';
  path_t result;
  result.data = path.data;
  result.len = 0;
  for (int j = path.len - (trailing ? 2 : 1); j >= 0; j--) {
    if (result.data[j] == '/') {
      result.len = j;
      break;
    }
  }
  if (result.len && trailing) {
    result.len++;
  }
  if (absolute && result.len == 0) {
    result.len = 1;
  }
  return result;
}

path_t path_extension(path_t path) {
  int end = path.len;
  if (path.data[end - 1] == '/') end--;
  int start = end;
  for (int i = end - 1; i >= 0; i--) {
    if (path.data[i] == '.') {
      start = i + 1;
      break;
    }
    if (path.data[i] == '/') break;
  }
  path_t result;
  result.data = path.data + start;
  result.len = end - start;
  return result;
}

path_t path_basename(path_t path) {
  int end = path.len;
  if (path.data[end - 1] == '/') end--;
  int start = 0;
  for (int i = end - 1; i >= 0; i--) {
    if (path.data[i] == '/') {
      start = i + 1;
      break;
    }
  }
  path_t result;
  result.data = path.data + start;
  result.len = end - start;
  return result;
}
