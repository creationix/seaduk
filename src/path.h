#ifndef PATH_H
#define PATH_H

const char* path_join(const char* a, const char* b);
const char* path_dirname(const char* path);
const char* path_extension(const char* path);
const char* path_filename(const char* path);

#endif
