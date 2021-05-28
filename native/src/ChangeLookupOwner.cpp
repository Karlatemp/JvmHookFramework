#include "lib.h"
#include <io_github_karlatemp_jvmhook_core_Bootstrap.h>
#include <iostream>
#include <cstring>

extern jvmtiEnv *jtiEnv;

jfieldID fieldid = null;

/*
 * Class:     io_github_karlatemp_jvmhook_core_Bootstrap
 * Method:    initLookupFieldOffset
 * Signature: (Ljava/lang/invoke/MethodHandles/Lookup;Ljava/lang/Class;)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_initLookupFieldOffset
        (JNIEnv *env, jclass, jobject lookup, jclass declaredClass) {
    auto lk = env->GetObjectClass(lookup);
    jint counts;
    jfieldID *fields;
    jtiEnv->GetClassFields(lk, &counts, &fields);
    for (jint i = 0; i < counts; i++) {
        char *field_name, *field_sign;
        jtiEnv->GetFieldName(lk, fields[i], &field_name, &field_sign, null);
        if (strcmp("Ljava/lang/Class;", field_sign) == 0) {
            if (env->IsSameObject(env->GetObjectField(lookup, fields[i]), declaredClass)) {
              //std::cout << field_name << '.' << field_sign << std::endl;
                fieldid = fields[i];
            }
        }
    }
    if (fieldid == null) {
        env->ThrowNew(env->FindClass("java/lang/NoSuchFieldError"), "Cannot found `lookupClass` field");
    }
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_Bootstrap
 * Method:    changeOwner
 * Signature: (Ljava/lang/invoke/MethodHandles/Lookup;Ljava/lang/Class;)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_changeOwner
        (JNIEnv *env, jclass, jobject lookup, jclass target) {
    env->SetObjectField(lookup, fieldid, target);
}

