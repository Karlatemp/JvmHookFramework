#ifndef NATIVE_UTILS_H
#define NATIVE_UTILS_H

#include <cstring>

size_t lateIndexOf(const char *string, char c);

size_t indexOf(const char *string, char c, size_t begin);

char readType(const char *desc, int *begin, int end);

#endif //NATIVE_UTILS_H
