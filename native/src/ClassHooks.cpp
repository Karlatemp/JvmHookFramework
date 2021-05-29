#include <cstdlib>
#include <memory.h>
#include <iostream>
#include "ClassHooks.h"
#include "jvmti.h"

extern jvmtiEnv *jtiEnv;

MethodHookInfo *newMethodHookInfo() {
    auto mh = new MethodHookInfo;
    memset(mh, 0, sizeof(MethodHookInfo));
    return mh;
}

MethodHookInfo *headHookInfo = newMethodHookInfo();
int methodHookCount = 0;

void registerHook(JNIEnv *jni, jclass klass, jmethodID method, jobject hook) {
    hook = jni->NewGlobalRef(hook);

    MethodHookInfo *currentInfo = headHookInfo;
    while (true) {
        if (currentInfo->method == null) {
            currentInfo->method = method;
            currentInfo->hook = hook;
            break;
        }
        auto next = currentInfo->next;
        if (next == null) {
            next = newMethodHookInfo();
            next->hook = hook;
            next->method = method;
            currentInfo->next = next;
            break;
        } else {
            currentInfo = next;
        }
    }
    methodHookCount++;
#ifdef DEBUG
    {
        jclass methodDeclaredClass;
        jtiEnv->GetMethodDeclaringClass(method, &methodDeclaredClass);
        char *methodDesc, *methodName, *methodClassName;
        jtiEnv->GetMethodName(method, &methodName, &methodDesc, null);
        jtiEnv->GetClassSignature(methodDeclaredClass, &methodClassName, null);
        jint modifiers;
        jtiEnv->GetMethodModifiers(method, &modifiers);

        std::cout << "[JvmHookFramework - registerHook   ] Registered hook into "
                  << methodClassName << "." << methodName << methodDesc
                  << ", hook count = " << methodHookCount
                  << std::endl;
    }
#endif
}

void unregisterHook(JNIEnv *jni, jclass klass, jmethodID method, jobject hook) {
    MethodHookInfo *currentInfo = headHookInfo;
    while (currentInfo != null) {

        if (*((void **) (currentInfo->method)) == *((void **) method)) {
            if (hook == null || jni->IsSameObject(currentInfo->hook, hook)) {
                currentInfo->method = null;
                jni->DeleteGlobalRef(currentInfo->hook);
                currentInfo->hook = null;
                methodHookCount--;
            }
        }

        currentInfo = currentInfo->next;
    }
}

