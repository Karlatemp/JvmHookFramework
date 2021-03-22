//
// Created by Karlatemp on 2021/3/19.
//

#ifndef NATIVE_LIB_H
#define NATIVE_LIB_H

#include <jni.h>
#include <jvmti.h>


#if __APPLE__
#define null 0
#else
#define null nullptr
#endif


#endif //NATIVE_LIB_H
