#include "utils.h"
#include <classfile_constants.h>
#include <iostream>

size_t lateIndexOf(const char *string, char c) {
    for (size_t i = strlen(string) - 1; i > 0; i--) {
        if (string[i] == c) return i;
    }
    return -1;
}

size_t indexOf(const char *string, char c, size_t begin) {
    size_t len = strlen(string);
    for (; begin < len; begin++) {
        if (string[begin] == c) return begin;
    }
    return -1;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

void setV(int *p, int v) {
    memcpy(p, &v, sizeof(int));
}

char readType(
        const char *desc,
        int *begin,
        int end
) {
    switch (desc[*begin]) {
        case JVM_SIGNATURE_ARRAY: {
            setV(begin, (*begin) + 1);
            readType(desc, begin, end);
            return JVM_SIGNATURE_ARRAY;
        }
        case JVM_SIGNATURE_BYTE:
        case JVM_SIGNATURE_CHAR:
        case JVM_SIGNATURE_FLOAT:
        case JVM_SIGNATURE_DOUBLE:
        case JVM_SIGNATURE_INT:
        case JVM_SIGNATURE_LONG:
        case JVM_SIGNATURE_SHORT:
        case JVM_SIGNATURE_BOOLEAN: {
            int i = *begin;
            setV(begin, (*begin) + 1);
            return desc[i];
        }
        case JVM_SIGNATURE_CLASS: {
          //std::cout << "CCB " << (*begin) << std::endl;
            auto ind = (int) indexOf(desc, ';', *begin);
          //std::cout << "CCI " << ind << std::endl;
            if (ind == -1) ind = end;
            setV(begin, ind + 1);
            return JVM_SIGNATURE_CLASS;
        }
    }
    return desc[*begin];
}

#pragma clang diagnostic pop
