#include "lib.h"
#include <io_github_karlatemp_jvmhook_core_LowLevelAccess.h>

extern jvmtiEnv *jtiEnv;

JNIEXPORT jobject JNICALL Java_io_github_karlatemp_jvmhook_core_LowLevelAccess_getClassLoader
        (JNIEnv *env, jobject, jclass k) {
    if (k == null) {
        env->ThrowNew(env->FindClass("java/lang/NullPointerException"), "class");
    }
    jobject cl;
    jtiEnv->GetClassLoader(k, &cl);
    return cl;
}
