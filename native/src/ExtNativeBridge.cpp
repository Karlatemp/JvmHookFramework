#include <io_github_karlatemp_jvmhook_core_extsys_NativeBridge.h>
#include <iostream>
#include "lib.h"

jobject extClassLoader = null;
extern jvmtiEnv *jtiEnv;

/*
 * Class:     io_github_karlatemp_jvmhook_core_extsys_NativeBridge
 * Method:    setExtClassLoader
 * Signature: (Ljava/lang/ClassLoader;)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_extsys_NativeBridge_setExtClassLoader
        (JNIEnv *env, jclass, jobject clo) {
    if (extClassLoader != null) {
        env->DeleteGlobalRef(extClassLoader);
    }
    extClassLoader = env->NewGlobalRef(clo);
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_extsys_NativeBridge
 * Method:    invokeLoad
 * Signature: (Ljava/lang/Class;)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_extsys_NativeBridge_invokeLoad
        (JNIEnv *env, jclass, jclass target) {
    auto m = env->GetStaticMethodID(target, "load", "()V");
    if (m != null) {
        env->CallStaticVoidMethod(target, m);
    } else {
        env->ExceptionClear();
        char *name;
        jtiEnv->GetClassSignature(target, &name, null);
        std::cerr << "[JvmHookFramework] [Extension] No method 'static void load()' in " << name << std::endl;
        std::cerr << "[JvmHookFramework] [Extension] This extension will not being loaded." << std::endl;
    }
}
