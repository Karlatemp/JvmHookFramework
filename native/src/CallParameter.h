#ifndef NATIVE_CALLPARAMETER_H
#define NATIVE_CALLPARAMETER_H

#include "jni.h"

class CallParameter {
public:
    char type;
    void *value;
    bool edited;

    jint iv;
    jint slot;
    jlong lv;
    jdouble jd;
    jfloat jf;
};


#endif //NATIVE_CALLPARAMETER_H
