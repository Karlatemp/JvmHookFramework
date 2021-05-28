#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-str34-c"

#include <io_github_karlatemp_jvmhook_core_ArgumentsNative.h>

#include "lib.h"
#include "CallParameter.h"
#include <classfile_constants.h>
#include <iostream>


static jvmtiEnv *jtiEnv = null;

jfieldID ArgumentsNativeClass$size = null;
jfieldID ArgumentsNativeClass$address = null;
jfieldID ArgumentsNativeClass$thisObj = null;
jmethodID ArgumentsNativeClass$init = null;

void init_ArgumentNative(JNIEnv *env, jvmtiEnv *jvmt) {
    jtiEnv = jvmt;

    auto ArgumentsNativeClass = env->FindClass("io/github/karlatemp/jvmhook/core/ArgumentsNative");
    ArgumentsNativeClass$size = env->GetFieldID(ArgumentsNativeClass, "size", "I");
    ArgumentsNativeClass$address = env->GetFieldID(ArgumentsNativeClass, "address", "J");
    ArgumentsNativeClass$thisObj = env->GetFieldID(ArgumentsNativeClass, "thisObj", "Ljava/lang/Object;");
    ArgumentsNativeClass$init = env->GetMethodID(ArgumentsNativeClass, "<init>", "()V");
    // std::cout << "SSx: " << ArgumentsNativeClass$init << std::endl;
    // std::cout << "SSvm: " << jtiEnv << std::endl;
}

CallParameter *getParameters(JNIEnv *env, jobject thiz) {
    auto f = env->GetLongField(thiz, ArgumentsNativeClass$address);
    if (f == 0) return null;
    return *((CallParameter **) f);
}

jint getSize(JNIEnv *env, jobject thiz) {
    return env->GetIntField(thiz, ArgumentsNativeClass$size);
}

void ile(JNIEnv *env) {
    env->ThrowNew(
            env->FindClass("java/lang/IllegalArgumentException"),
            null
    );
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    getAsInt
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_getAsInt
        (JNIEnv *env, jobject thiz, jint index) {
    if (index < 0) return -1;
    auto p = getParameters(env, thiz);
    if (p == null) return -1;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_INT) {
            ile(env);
        } else {
            return px.iv;
        }
    }
    return -1;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    getAsByte
 * Signature: (I)B
 */
JNIEXPORT jbyte JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_getAsByte
        (JNIEnv *env, jobject thiz, jint index) {
    if (index < 0) return -1;
    auto p = getParameters(env, thiz);
    if (p == null) return -1;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_INT) {
            ile(env);
        } else {
            return px.iv;
        }
    }
    return -1;
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    getAsDouble
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_getAsDouble
        (JNIEnv *env, jobject thiz, jint index) {
    if (index < 0) return -1;
    auto p = getParameters(env, thiz);
    if (p == null) return -1;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
      //std::cout << "SST: " << px.type << " I " << index << std::endl;
        if (px.type != JVM_SIGNATURE_DOUBLE) {
            ile(env);
        } else {
            return px.jd;
        }
    }
    return -1;
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    getAsFloat
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_getAsFloat
        (JNIEnv *env, jobject thiz, jint index) {
    if (index < 0) return -1;
    auto p = getParameters(env, thiz);
    if (p == null) return -1;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_FLOAT) {
            ile(env);
        } else {
            return px.jf;
        }
    }
    return -1;
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    getAsShort
 * Signature: (I)S
 */
JNIEXPORT jshort JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_getAsShort
        (JNIEnv *env, jobject thiz, jint index) {
    if (index < 0) return -1;
    auto p = getParameters(env, thiz);
    if (p == null) return -1;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_INT) {
            ile(env);
        } else {
            return px.iv;
        }
    }
    return -1;
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    getAsBoolean
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_getAsBoolean
        (JNIEnv *env, jobject thiz, jint index) {
    if (index < 0) return 0;
    auto p = getParameters(env, thiz);
    if (p == null) return 0;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_INT) {
            ile(env);
        } else {
            return px.iv;
        }
    }
    return 0;
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    getAsLong
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_getAsLong
        (JNIEnv *env, jobject thiz, jint index) {
    if (index < 0) return -1;
    auto p = getParameters(env, thiz);
    if (p == null) return -1;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_LONG) {
            ile(env);
        } else {
            return px.lv;
        }
    }
    return -1;
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    getAsChar
 * Signature: (I)C
 */
JNIEXPORT jchar JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_getAsChar
        (JNIEnv *env, jobject thiz, jint index) {
    if (index < 0) return -1;
    auto p = getParameters(env, thiz);
    if (p == null) return -1;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_INT) {
            ile(env);
        } else {
            return px.iv;
        }
    }
    return -1;
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    getAsObject
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_getAsObject
        (JNIEnv *env, jobject thiz, jint index) {
    if (index < 0) return null;
    auto p = getParameters(env, thiz);
    if (p == null) return null;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_CLASS) {
            ile(env);
        } else {
            return (jobject) px.value;
        }
    }
    return null;

}


/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    putByte
 * Signature: (IB)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_putByte
        (JNIEnv *env, jobject thiz, jint index, jbyte value) {
    if (index < 0) return;
    auto p = getParameters(env, thiz);
    if (p == null) return;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_INT) {
            ile(env);
        } else {
            px.edited = true;
            px.iv = (jint) value;
            p[index] = px;
        }
    }
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    putFloat
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_putFloat
        (JNIEnv *env, jobject thiz, jint index, jfloat value) {
    if (index < 0) return;
    auto p = getParameters(env, thiz);
    if (p == null) return;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_FLOAT) {
            ile(env);
        } else {
            px.edited = true;
            px.jf = value;
            p[index] = px;
        }
    }
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    putShort
 * Signature: (IS)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_putShort
        (JNIEnv *env, jobject thiz, jint index, jshort value) {
    if (index < 0) return;
    auto p = getParameters(env, thiz);
    if (p == null) return;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_INT) {
            ile(env);
        } else {
            px.edited = true;
            px.iv = (jint) value;
            p[index] = px;
        }
    }
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    putBoolean
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_putBoolean
        (JNIEnv *env, jobject thiz, jint index, jboolean value) {
    if (index < 0) return;
    auto p = getParameters(env, thiz);
    if (p == null) return;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_INT) {
            ile(env);
        } else {
            px.edited = true;
            px.iv = (jint) value;
            p[index] = px;
        }
    }
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    putDouble
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_putDouble
        (JNIEnv *env, jobject thiz, jint index, jdouble value) {
    if (index < 0) return;
    auto p = getParameters(env, thiz);
    if (p == null) return;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_DOUBLE) {
            ile(env);
        } else {
            px.edited = true;
            px.jd = value;
            p[index] = px;
        }
    }
}
/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    putLong
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_putLong
        (JNIEnv *env, jobject thiz, jint index, jlong value) {
    if (index < 0) return;
    auto p = getParameters(env, thiz);
    if (p == null) return;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_LONG) {
            ile(env);
        } else {
            px.edited = true;
            px.lv = value;
            p[index] = px;
        }
    }
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    putChar
 * Signature: (IC)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_putChar
        (JNIEnv *env, jobject thiz, jint index, jchar value) {
    if (index < 0) return;
    auto p = getParameters(env, thiz);
    if (p == null) return;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_INT) {
            ile(env);
        } else {
            px.edited = true;
            px.iv = (jint) value;
            p[index] = px;
        }
    }
}

/*
 * Class:     io_github_karlatemp_jvmhook_core_ArgumentsNative
 * Method:    putObject
 * Signature: (ILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_ArgumentsNative_putObject
        (JNIEnv *env, jobject thiz, jint index, jobject value) {
    if (index < 0) return;
    auto p = getParameters(env, thiz);
    if (p == null) return;
    if (index < getSize(env, thiz)) {
        auto px = p[index];
        if (px.type != JVM_SIGNATURE_CLASS) {
            ile(env);
        } else {
            if (px.edited) {
                env->DeleteGlobalRef((jobject) px.value);
            }

            px.edited = true;
            px.value = env->NewGlobalRef(value);
            p[index] = px;
        }
    }
}


#pragma clang diagnostic pop
