#include "jni.h"
#include "jvmti.h"
#include "lib.h"
#include "io_github_karlatemp_jvmhook_core_Bootstrap_JvmHookFrameworkBridge.h"

extern jvmtiEnv *jtiEnv;

/*
 * Class:     io_github_karlatemp_jvmhook_core_Bootstrap_JvmHookFrameworkBridge
 * Method:    getMethodDesc
 * Signature: (Ljava/lang/reflect/Method;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_00024JvmHookFrameworkBridge_getMethodDesc
        (JNIEnv *env, jobject, jobject met) {
    auto met0 = env->FromReflectedMethod(met);
    char *desc;

    auto c = jtiEnv->GetMethodName(met0, null, &desc, null);
    if (c != JNI_OK) {
        env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "JNI Response not JNI_OK");
        return null;
    }
    return env->NewStringUTF(desc);
}
