#include <io_github_karlatemp_jvmhook_core_JvmReturnValueImpl.h>
#include "lib.h"
#include "jvmti.h"


jfieldID JvmReturnValueImpl$address;
jmethodID JvmReturnValueImpl$init;

void init_JvmReturnValueImpl(JNIEnv *jniEnv, jvmtiEnv *jvmti_env) {
    auto c = jniEnv->FindClass("io/github/karlatemp/jvmhook/core/JvmReturnValueImpl");
    JvmReturnValueImpl$address = jniEnv->GetFieldID(c, "address", "J");
    JvmReturnValueImpl$init = jniEnv->GetMethodID(c, "<init>", "(Ljava/lang/Throwable;Z)V");
}

jobject new_JvmReturnValueImpl(
        JNIEnv *jniEnv, jvmtiEnv *jvmti_env,
        jboolean was_popped_by_exception,
        jobject jthr,
        jvalue *pointer
) {
    jclass c;
    jvmti_env->GetMethodDeclaringClass(JvmReturnValueImpl$init, &c);
    auto o = jniEnv->NewObject(c, JvmReturnValueImpl$init, jthr, was_popped_by_exception);
    jniEnv->SetLongField(o, JvmReturnValueImpl$address, (jlong) pointer);
    return o;
}

void release_JvmReturnValueImpl(JNIEnv *jniEnv, jobject obj) {
    jniEnv->SetLongField(obj, JvmReturnValueImpl$address, 0);
}

jvalue *getJv(JNIEnv *env, jobject obj) {
    return (jvalue *) env->GetLongField(obj, JvmReturnValueImpl$address);
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_JvmReturnValueImpl
 * Method:    asInt
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_io_github_karlatemp_jvmhook_core_JvmReturnValueImpl_asInt
        (JNIEnv *e, jobject o) {
    auto v = getJv(e, o);
    if (v == null) return 0;
    return (*v).i;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_JvmReturnValueImpl
 * Method:    asByte
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_io_github_karlatemp_jvmhook_core_JvmReturnValueImpl_asByte
        (JNIEnv *e, jobject o) {
    auto v = getJv(e, o);
    if (v == null) return 0;
    return (*v).b;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_JvmReturnValueImpl
 * Method:    asBoolean
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_io_github_karlatemp_jvmhook_core_JvmReturnValueImpl_asBoolean
        (JNIEnv *e, jobject o) {
    auto v = getJv(e, o);
    if (v == null) return 0;
    return (*v).z;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_JvmReturnValueImpl
 * Method:    asChar
 * Signature: ()C
 */
JNIEXPORT jchar JNICALL Java_io_github_karlatemp_jvmhook_core_JvmReturnValueImpl_asChar
        (JNIEnv *e, jobject o) {
    auto v = getJv(e, o);
    if (v == null) return 0;
    return (*v).c;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_JvmReturnValueImpl
 * Method:    asShort
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_io_github_karlatemp_jvmhook_core_JvmReturnValueImpl_asShort
        (JNIEnv *e, jobject o) {
    auto v = getJv(e, o);
    if (v == null) return 0;
    return (*v).s;
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_JvmReturnValueImpl
 * Method:    asLong
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_io_github_karlatemp_jvmhook_core_JvmReturnValueImpl_asLong
        (JNIEnv *e, jobject o) {
    auto v = getJv(e, o);
    if (v == null) return 0;
    return (*v).j;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_JvmReturnValueImpl
 * Method:    asFloat
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_io_github_karlatemp_jvmhook_core_JvmReturnValueImpl_asFloat
        (JNIEnv *e, jobject o) {
    auto v = getJv(e, o);
    if (v == null) return 0;
    return (*v).f;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_JvmReturnValueImpl
 * Method:    asDouble
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_io_github_karlatemp_jvmhook_core_JvmReturnValueImpl_asDouble
        (JNIEnv *e, jobject o) {
    auto v = getJv(e, o);
    if (v == null) return 0;
    return (*v).d;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_JvmReturnValueImpl
 * Method:    asObject
 * Signature: ()Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_io_github_karlatemp_jvmhook_core_JvmReturnValueImpl_asObject
        (JNIEnv *e, jobject o) {
    auto v = getJv(e, o);
    if (v == null) return null;
    return (*v).l;
}
