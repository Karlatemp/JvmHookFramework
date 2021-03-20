#include <jni.h>
#include "lib.h"
#include <jvmti.h>
#include <classfile_constants.h>
#include <io_github_karlatemp_jvmhook_core_Bootstrap.h>
#include <memory.h>
#include <iostream>
#include <cstring>
#include "utils.h"
#include "CallParameter.h"


static JavaVM *javaVm = nullptr;
static jvmtiEnv *jtiEnv = nullptr;

extern jfieldID ArgumentsNativeClass$size;
extern jfieldID ArgumentsNativeClass$address;
extern jfieldID ArgumentsNativeClass$thisObj;
extern jmethodID ArgumentsNativeClass$init;

static jobject classLoaderX;
static jmethodID shouldHookMethodEnter;

static jmethodID broadcastMethodEnter;


static jmethodID methodCallImplInit;

void init_ArgumentNative(JNIEnv *env, jvmtiEnv *jtiEnv);

void init_ForceEarlyReturnNative(JNIEnv *env, jvmtiEnv *jtiEnv);

jobject newForceEarlyReturnNative(JNIEnv *env, CallParameter *pointer);

jclass getDeclaredClass(jmethodID met) {
    jclass c;
    jtiEnv->GetMethodDeclaringClass(met, &c);
    return c;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-misplaced-widening-cast"

JNICALL void onMethodEntry(jvmtiEnv *jvmti_env,
                   JNIEnv *jni_env,
                   jthread thread,
                   jmethodID method) {

    jobject classLoaderZ;
    jclass methodOwner;
    jvmti_env->GetMethodDeclaringClass(method, &methodOwner);
    jvmti_env->GetClassLoader(methodOwner, &classLoaderZ);
    if (classLoaderZ == nullptr) return;
    if (jni_env->IsSameObject(classLoaderZ, classLoaderX)) return;

    char *cln, *methodDesc, *methodName;
    jvmti_env->GetClassSignature(methodOwner, &cln, nullptr);
    jvmti_env->GetMethodName(method, &methodName, &methodDesc, nullptr);
    jint modifiers;
    jvmti_env->GetMethodModifiers(method, &modifiers);
    jstring metName0 = jni_env->NewStringUTF(methodName);
    jstring metDesc0 = jni_env->NewStringUTF(methodDesc);

    //std::cout << "[NATIVE] Method " << cln << methodName << methodDesc << " invoked." << std::endl;

    jclass bridgeClass;
    jvmti_env->GetMethodDeclaringClass(shouldHookMethodEnter, &bridgeClass);


    jboolean hook = jni_env->CallStaticBooleanMethod(
            bridgeClass, shouldHookMethodEnter,
            methodOwner, classLoaderZ,
            metName0, metDesc0, modifiers
    );
    //std::cout << "HOOK CALL: " << hook << std::endl;
    if (hook) {
        int slots = 0, size = 0;
        size_t end = indexOf(methodDesc, ')', 1);
        for (int i = 1; i < end;) {
            readType(methodDesc, &i, end);
            size++;
        }
        //std::cout << "SZZS " << slots << ", " << size << std::endl;
        auto *parameters = static_cast<CallParameter *>(malloc(size * sizeof(CallParameter)));

        //std::cout << "PMAL" << std::endl;
        memset(parameters, 0, size * sizeof(CallParameter));
        //std::cout << "PMST" << std::endl;

        for (int i = 1, x = 0; i < end; x++) {
            auto t = readType(methodDesc, &i, end);

            switch (t) { // NOLINT(hicpp-multiway-paths-covered)
                case JVM_SIGNATURE_ARRAY:
                case JVM_SIGNATURE_CLASS: {

                    auto p = CallParameter();
                    p.type = JVM_SIGNATURE_CLASS;
                    jobject obj;
                    jvmti_env->GetLocalObject(thread, 0, slots, &obj);
                    p.value = obj;
                    p.slot = slots;
                    p.edited = false;
                    parameters[x] = p;

                    slots++;
                    break;
                }
                case JVM_SIGNATURE_BYTE:
                case JVM_SIGNATURE_CHAR:
                case JVM_SIGNATURE_INT:
                case JVM_SIGNATURE_SHORT:
                case JVM_SIGNATURE_BOOLEAN: {

                    auto p = CallParameter();
                    p.type = JVM_SIGNATURE_INT;
                    jint val;
                    jvmti_env->GetLocalInt(thread, 0, slots, &val);
                    p.iv = val;
                    p.slot = slots;
                    p.edited = false;
                    parameters[x] = p;

                    slots++;
                    break;
                }
                case JVM_SIGNATURE_FLOAT: {

                    auto p = CallParameter();
                    p.type = JVM_SIGNATURE_FLOAT;
                    jfloat val;
                    jvmti_env->GetLocalFloat(thread, 0, slots, &val);
                    p.jf = val;
                    p.slot = slots;
                    p.edited = false;
                    parameters[x] = p;

                    slots++;
                    break;
                }
                case JVM_SIGNATURE_LONG: {

                    auto p = CallParameter();
                    p.type = JVM_SIGNATURE_LONG;
                    jlong val;
                    jvmti_env->GetLocalLong(thread, 0, slots, &val);
                    p.lv = val;
                    p.slot = slots;
                    p.edited = false;
                    parameters[x] = p;

                    slots += 2;
                    break;
                }
                case JVM_SIGNATURE_DOUBLE: {
                    auto p = CallParameter();
                    p.type = JVM_SIGNATURE_DOUBLE;
                    jdouble val;
                    jvmti_env->GetLocalDouble(thread, 0, slots, &val);
                    p.jd = val;
                    p.slot = slots;
                    p.edited = false;
                    parameters[x] = p;

                    slots += 2;
                    break;
                }
            }

        }

        jclass c = getDeclaredClass(methodCallImplInit);
        //std::cout << "FFC " << ArgumentsNativeClass$init << std::endl;
        jclass ancc = getDeclaredClass(ArgumentsNativeClass$init);
        //std::cout << "JC " << c << std::endl;

        auto anc = jni_env->NewObject(
                ancc,
                ArgumentsNativeClass$init
        );

        jobject thisObj;
        if ((modifiers & JVM_ACC_STATIC) == 0) {
            jvmti_env->GetLocalInstance(thread, 0, &thisObj);
        } else {
            thisObj = nullptr;
        }

        jni_env->SetIntField(anc, ArgumentsNativeClass$size, size);
        jni_env->SetLongField(anc, ArgumentsNativeClass$address, (jlong) (&parameters));
        if (thisObj != nullptr) {
            //std::cout << "TOBJ " << ArgumentsNativeClass$thisObj << std::endl;
            jni_env->SetObjectField(anc, ArgumentsNativeClass$thisObj, thisObj);
            //std::cout << "OXAV " << ArgumentsNativeClass$thisObj << std::endl;
        }

        CallParameter returnForce = CallParameter();
        returnForce.edited = false;

        //std::cout << "S2Bx " << methodCallImplInit << std::endl;
        auto call = jni_env->NewObject(
                c,
                methodCallImplInit,
                methodOwner, metName0, metDesc0,
                modifiers, anc,
                newForceEarlyReturnNative(jni_env, &returnForce)
        );
        //std::cout << "S2B" << std::endl;

        jni_env->CallStaticVoidMethod(bridgeClass, broadcastMethodEnter, call);

        //std::cout << "SB" << std::endl;
        jni_env->SetLongField(anc, ArgumentsNativeClass$address, (jlong) 0);
        //std::cout << "SMB" << std::endl;
        for (int i = 0; i < size; i++) {
            auto p = parameters[i];
            if (p.edited) {
                switch (p.type) {
                    case JVM_SIGNATURE_INT: {
                        jvmti_env->SetLocalInt(thread, 0, p.slot, p.iv);
                        break;
                    }
                    case JVM_SIGNATURE_FLOAT: {
                        jvmti_env->SetLocalFloat(thread, 0, p.slot, p.jf);
                        break;
                    }
                    case JVM_SIGNATURE_DOUBLE: {
                        jvmti_env->SetLocalDouble(thread, 0, p.slot, p.jd);
                        break;
                    }
                    case JVM_SIGNATURE_CLASS: {
                        auto jo = (jobject) p.value;
                        jvmti_env->SetLocalObject(thread, 0, p.slot, jo);
                        jni_env->DeleteGlobalRef(jo);
                        break;
                    }
                    case JVM_SIGNATURE_LONG: {
                        jvmti_env->SetLocalLong(thread, 0, p.slot, p.lv);
                        break;
                    }
                }
            }
        }
        //std::cout << "S2" << std::endl;
        if (returnForce.edited) {
            // std::cout << "RF EDX " << methodDesc[end + 1] << std::endl;
            switch (methodDesc[end + 1]) {
                case JVM_SIGNATURE_CLASS:
                case JVM_SIGNATURE_ARRAY: {
                    auto o = (jobject) returnForce.value;
                    jvmti_env->ForceEarlyReturnObject(thread, o);
                    jni_env->DeleteGlobalRef(o);
                    break;
                }
                case JVM_SIGNATURE_INT:
                case JVM_SIGNATURE_SHORT:
                case JVM_SIGNATURE_CHAR:
                case JVM_SIGNATURE_BOOLEAN:
                case JVM_SIGNATURE_BYTE: {
                    jvmti_env->ForceEarlyReturnInt(thread, returnForce.iv);
                    break;
                }
                case JVM_SIGNATURE_VOID: {
                    jvmti_env->ForceEarlyReturnVoid(thread);
                    break;
                }
                case JVM_SIGNATURE_FLOAT: {
                    jvmti_env->ForceEarlyReturnFloat(thread, returnForce.jf);
                    break;
                }
                case JVM_SIGNATURE_DOUBLE: {
                    jvmti_env->ForceEarlyReturnDouble(thread, returnForce.jd);
                    break;
                }
                case JVM_SIGNATURE_LONG: {
                    jvmti_env->ForceEarlyReturnLong(thread, returnForce.lv);
                    break;
                }
            }
        }
        free(parameters);
        //std::cout << "S3" << std::endl;

    }

    //std::cout << "OOE" << std::endl;

}

#pragma clang diagnostic pop

JNICALL void onMethodExit(jvmtiEnv *jvmti_env,
                  JNIEnv *jni_env,
                  jthread thread,
                  jmethodID method,
                  jboolean was_popped_by_exception,
                  jvalue return_value) {
    // TODO
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
    //std::cout << "OnLoad called" << std::endl;

    javaVm = vm;
    jint jnierror = vm->GetEnv((void **) &jtiEnv,
                               JVMTI_VERSION_1_2);
    if (jnierror != JNI_OK) {
        std::cerr << "Get JvmTI failed" << std::endl;
        return jnierror;
    }
    jvmtiCapabilities capabilities;
    jtiEnv->GetCapabilities(&capabilities);
    capabilities.can_generate_method_entry_events = true;
    capabilities.can_generate_method_exit_events = true;
    capabilities.can_force_early_return = true;
    // capabilities.can_generate_native_method_bind_events = true;
    capabilities.can_access_local_variables = true;
    jint cresp = jtiEnv->AddCapabilities(&capabilities);
    if (cresp != JNI_OK) {
        std::cerr << "AddCapabilities failed: " << cresp << std::endl;
        return cresp;
    }

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof callbacks);
    callbacks.MethodEntry = onMethodEntry;
    callbacks.MethodExit = onMethodExit;
    if (jtiEnv->SetEventCallbacks(&callbacks, sizeof callbacks) != JNI_OK) {
        std::cerr << "Set callbacks failed" << std::endl;
        return JVMTI_ERROR_INTERNAL;
    }

    return JVMTI_ERROR_NONE;

}


JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_initializeNative(JNIEnv *env, jclass owner) {
    if (jtiEnv == nullptr) {
        env->GetJavaVM(&javaVm);
        Agent_OnLoad(javaVm, nullptr, nullptr);
    }
    jtiEnv->GetClassLoader(owner, &classLoaderX);

    classLoaderX = env->NewGlobalRef(classLoaderX);

    //shouldHookMethodEnter(Ljava/lang/Class;Ljava/lang/ClassLoader;Ljava/lang/String;Ljava/lang/String;I)Z
    shouldHookMethodEnter = env->GetStaticMethodID(
            owner, "shouldHookMethodEnter",
            "(Ljava/lang/Class;Ljava/lang/ClassLoader;Ljava/lang/String;Ljava/lang/String;I)Z"
    );

    //
    {
        broadcastMethodEnter = env->GetStaticMethodID(
                owner, "broadcastMethodEnter",
                "(Lio/github/karlatemp/jvmhook/call/MethodCall;)V"
        );

        jclass MethodCallImpl = env->FindClass("io/github/karlatemp/jvmhook/core/MethodCallImpl");
        methodCallImplInit = env->GetMethodID(
                MethodCallImpl, "<init>",
                "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/String;ILio/github/karlatemp/jvmhook/call/Arguments;Lio/github/karlatemp/jvmhook/call/MethodCall$ForceEarlyReturn;)V"
        );

    }
    init_ArgumentNative(env, jtiEnv);
    init_ForceEarlyReturnNative(env, jtiEnv);

    jtiEnv->SetEventNotificationMode(
            JVMTI_ENABLE,
            JVMTI_EVENT_METHOD_ENTRY,
            nullptr /* all threads */);
    jtiEnv->SetEventNotificationMode(
            JVMTI_ENABLE,
            JVMTI_EVENT_METHOD_EXIT,
            nullptr /* all threads */);
}
