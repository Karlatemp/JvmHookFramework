
#include "lib.h"
#include <io_github_karlatemp_jvmhook_core_MethodInfoImpl.h>
#include <iostream>
#include "jvmti.h"
#include "classfile_constants.h"

extern jvmtiEnv *jtiEnv;

jfieldID pointerField = null;
jfieldID cField = null;
jfieldID modiField = null;
jmethodID init = null;


void init_MethodInfoImpl(JNIEnv *jni) {
    if (pointerField == null) {
        auto c = jni->FindClass("io/github/karlatemp/jvmhook/core/MethodInfoImpl");
        pointerField = jni->GetFieldID(c, "pointer", "J");
        cField = jni->GetFieldID(c, "c", "Ljava/lang/Class;");
        modiField = jni->GetFieldID(c, "modi", "I");
        init = jni->GetMethodID(c, "<init>", "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/String;I)V");
    }
}

jobject new_MethodInfoImpl(
        JNIEnv *jni,
        jvmtiEnv *jrtEnv,
        jclass c,
        jmethodID *m,
        jstring n,
        jstring desc,
        jint mod) {

    jclass own;
    jrtEnv->GetMethodDeclaringClass(init, &own);

    auto o = jni->NewObject(own, init, c, n, desc, mod);
    jni->SetLongField(o, pointerField, (jlong) m);
    return o;
}

void release_MethodInfoImpl(JNIEnv *jni, jobject o) {
    jni->SetLongField(o, pointerField, 0);
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_MethodInfoImpl
 * Method:    toMethod
 * Signature: ()Ljava/lang/reflect/Method;
 */
JNIEXPORT jobject JNICALL Java_io_github_karlatemp_jvmhook_core_MethodInfoImpl_toMethod
        (JNIEnv *jni, jobject o) {
    auto v = jni->GetLongField(o, pointerField);
    if (v == 0) return null;
    jmethodID met = *((jmethodID *) v);
    return jni->ToReflectedMethod(
            (jclass) jni->GetObjectField(o, cField),
            met,
            (jni->GetIntField(o, modiField) & JVM_ACC_STATIC) != 0
    );
}

