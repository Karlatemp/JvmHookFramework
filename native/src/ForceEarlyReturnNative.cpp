#include "lib.h"
#include "io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative.h"
#include <jni.h>
#include "CallParameter.h"

static jvmtiEnv *jrtEnv;
jmethodID ForceEarlyReturnNative$init;
jfieldID ForceEarlyReturnNative$address;

void init_ForceEarlyReturnNative(JNIEnv *env, jvmtiEnv *jvmt) {
    jrtEnv = jvmt;

    auto c = env->FindClass("io/github/karlatemp/jvmhook/core/ForceEarlyReturnNative");
    ForceEarlyReturnNative$init = env->GetMethodID(c, "<init>", "()V");
    ForceEarlyReturnNative$address = env->GetFieldID(c, "address", "J");
}

jobject newForceEarlyReturnNative(JNIEnv *env, CallParameter *pointer) {
    jclass c;
    jrtEnv->GetMethodDeclaringClass(ForceEarlyReturnNative$init, &c);
    auto o = env->NewObject(c, ForceEarlyReturnNative$init);
    env->SetLongField(o, ForceEarlyReturnNative$address, (jlong) pointer);
    return o;
}

void clearForceEarlyReturnNativeAddress(JNIEnv *env, jobject o) {
    env->SetLongField(o, ForceEarlyReturnNative$address, 0);
}

CallParameter *getCP(JNIEnv *env, jobject thiz) {
    return (CallParameter *) env->GetLongField(thiz, ForceEarlyReturnNative$address);
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative
 * Method:    returnVoid
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative_returnVoid
        (JNIEnv *env, jobject thiz) {
    auto c = getCP(env, thiz);
    if (c == null)return;
    c->edited = true;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative
 * Method:    returnInt
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative_returnInt
        (JNIEnv *env, jobject thiz, jint value) {
    auto c = getCP(env, thiz);
    if (c == null)return;
    c->edited = true;
    c->iv = value;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative
 * Method:    returnFloat
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative_returnFloat
        (JNIEnv *env, jobject thiz, jfloat value) {
    auto c = getCP(env, thiz);
    if (c == null)return;
    c->edited = true;
    c->jf = value;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative
 * Method:    returnDouble
 * Signature: (F)D
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative_returnDouble
        (JNIEnv *env, jobject thiz, jdouble value) {
    auto c = getCP(env, thiz);
    if (c == null)return;
    c->edited = true;
    c->jf = value;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative
 * Method:    returnLong
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative_returnLong
        (JNIEnv *env, jobject thiz, jlong value) {
    auto c = getCP(env, thiz);
    if (c == null)return;
    c->edited = true;
    c->lv = value;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative
 * Method:    returnObject
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ForceEarlyReturnNative_returnObject
        (JNIEnv *env, jobject thiz, jobject value) {
    auto c = getCP(env, thiz);
    if (c == null)return;
    if (c->edited) {
        env->DeleteGlobalRef((jobject) (c->value));
    }
    c->edited = true;
    c->value = env->NewGlobalRef(value);
}

