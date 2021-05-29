//
// Created by Karlatemp on 2021/5/29.
//

#ifndef NATIVE_CLASSHOOKS_H
#define NATIVE_CLASSHOOKS_H

#include "lib.h"
#include <vector>

struct MethodHookInfo {
    jmethodID method;
    jobject hook;
    MethodHookInfo *next;
};

MethodHookInfo *newMethodHookInfo();

void registerHook(JNIEnv *jni, jclass klass, jmethodID method, jobject hook);

void unregisterHook(JNIEnv *jni, jclass klass, jmethodID method, jobject hook);


#endif //NATIVE_CLASSHOOKS_H
