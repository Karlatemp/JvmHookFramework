#include <jni.h>
#include "lib.h"
#include <jvmti.h>
#include <classfile_constants.h>
#include <io_github_karlatemp_jvmhook_core_Bootstrap.h>
#include <memory.h>
#include <iostream>
#include "utils.h"
#include "CallParameter.h"
#include <vector>
#include <algorithm>
#include "ClassHooks.h"


static JavaVM *javaVm = null;
jvmtiEnv *jtiEnv = null;

// region fields

extern jfieldID ArgumentsNativeClass$size;
extern jfieldID ArgumentsNativeClass$address;
extern jfieldID ArgumentsNativeClass$thisObj;
extern jmethodID ArgumentsNativeClass$init;

static jobject classLoaderX;

static jmethodID broadcastMethodEnter;
static jmethodID broadcastMethodExit;
static jmethodID broadcastClassPrepare;


static jmethodID methodCallImplInit;
static jfieldID methodCallImpl$caller;

// endregion

extern jobject extClassLoader;

bool shouldNotHookJvmClasses = true;

extern MethodHookInfo *headHookInfo;
extern int methodHookCount;

struct HookInvokingInfo {
    bool runningHook;
    bool exitingMethod; // onMethodExit processing
    bool broadcastingMethodPrepareLoad;

    int hookCounts;
    jobject *hooks;
    jmethodID targetMethod;
    jint modifiers;
    jstring methodName;
    jstring methodDesc;
};

// region initializers for other sources

void init_ArgumentNative(JNIEnv *env, jvmtiEnv *jtiEnv);

void init_ForceEarlyReturnNative(JNIEnv *env, jvmtiEnv *jtiEnv);

void init_JvmReturnValueImpl(JNIEnv *jniEnv, jvmtiEnv *jvmti_env);

jobject newForceEarlyReturnNative(JNIEnv *env, CallParameter *pointer);

void clearForceEarlyReturnNativeAddress(JNIEnv *env, jobject);

jobject new_JvmReturnValueImpl(
        JNIEnv *jniEnv, jvmtiEnv *jvmti_env,
        jboolean was_popped_by_exception, jobject jthr,
        jvalue *pointer
);

void release_JvmReturnValueImpl(JNIEnv *jniEnv, jobject obj);

void init_MethodInfoImpl(JNIEnv *jni);

jobject new_MethodInfoImpl(
        JNIEnv *, jvmtiEnv *, jclass, jmethodID *,
        jstring, jstring, jint
);

void release_MethodInfoImpl(JNIEnv *jni, jobject o);

// endregion

jclass getDeclaredClass(jmethodID met) {
    jclass c;
    jtiEnv->GetMethodDeclaringClass(met, &c);
    return c;
}

bool shouldRunHook(
        JNIEnv *jni, jmethodID metid,
        int *hookCounts, jobject **allHooks
) {

    int hccount = 0;
    jobject *allHooks0 = null;

    auto mh = headHookInfo;
    do {
        if (mh->method != null) {
            if (*((void **) (mh->method)) == *((void **) (metid))) {
                if (allHooks0 == null) {
                    allHooks0 = static_cast<jobject *>(malloc(methodHookCount * (sizeof(jobject))));
                    *allHooks = allHooks0;
                }
                allHooks0[hccount] = mh->hook;
                hccount++;
            }
        }
        mh = mh->next;
    } while (mh != null);

    *hookCounts = hccount;
    if (hccount != 0) return true;
    *allHooks = null;
    free(allHooks0);
    return false;
}

HookInvokingInfo *getHookInvokingInfo(jvmtiEnv *jvmti_env, jthread thread) {
    HookInvokingInfo *info;
    jvmti_env->GetThreadLocalStorage(thread, reinterpret_cast<void **>(&info));
    if (info == null) {
        info = new HookInvokingInfo;
        memset(info, 0, sizeof(HookInvokingInfo));
        // std::cout << "NTT HII " << std::endl;
        jvmti_env->SetThreadLocalStorage(thread, info);
    }
    return info;
}

jobject findCaller(jvmtiEnv *jvmti_env, JNIEnv *jniEnv, jthread thread) {
    jvmtiFrameInfo frameInfo[1];
    jint frameCount;
    //std::cout << "Caller invoke" << std::endl;
    auto err0 = jvmti_env->GetStackTrace(thread, 1, 1, frameInfo, &frameCount);
    //std::cout << "Caller invoke complete, " << frameCount << std::endl;
    if (err0 == JVMTI_ERROR_NONE && frameCount > 0) {
        jclass resp = null;
        //std::cout << "Fc" << std::endl;
        jvmti_env->GetMethodDeclaringClass(frameInfo[0].method, &resp);
        //std::cout << "EOF" << std::endl;
        return resp;
    } else {
        //std::cout << "JTI: " << err0 << std::endl;
    }
    return null;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-misplaced-widening-cast"

JNICALL void onMethodEntry(
        jvmtiEnv *jvmti_env,
        JNIEnv *jni_env,
        jthread thread,
        jmethodID method
) {
    int hookCount;
    jobject *hookInstances;
    bool hook = shouldRunHook(jni_env, method, &hookCount, &hookInstances);
    if (!hook) return;

    auto invokingInfo = getHookInvokingInfo(jvmti_env, thread);
    if (invokingInfo->runningHook || invokingInfo->exitingMethod) {
        return;
    }

    jobject classLoaderZ;
    jclass methodOwner;
    jvmti_env->GetMethodDeclaringClass(method, &methodOwner);
    jvmti_env->GetClassLoader(methodOwner, &classLoaderZ);
    if (jni_env->IsSameObject(classLoaderZ, classLoaderX)) return;
    if (extClassLoader != null && jni_env->IsSameObject(classLoaderZ, extClassLoader)) return;

    invokingInfo->runningHook = true;
    {

        char *cln, *methodDesc, *methodName;
        jvmti_env->GetClassSignature(methodOwner, &cln, null);
        jvmti_env->GetMethodName(method, &methodName, &methodDesc, null);
        jint modifiers;
        jvmti_env->GetMethodModifiers(method, &modifiers);


#ifdef DEBUG
        std::cout << "[JvmHookFramework - onMethodEntry  ] "
                  << cln << "." << methodName << methodDesc
                  << std::endl;
#endif
        invokingInfo->targetMethod = method;
        invokingInfo->hookCounts = hookCount;

        jstring metName0 = jni_env->NewStringUTF(methodName);
        jstring metDesc0 = jni_env->NewStringUTF(methodDesc);
        invokingInfo->methodName = metName0;
        invokingInfo->methodDesc = metDesc0;

        jclass bridgeClass;
        jvmti_env->GetMethodDeclaringClass(broadcastMethodEnter, &bridgeClass);


        int slots = 0, size = 0;
        if ((modifiers & JVM_ACC_STATIC) == 0) { slots++; }
        size_t end = indexOf(methodDesc, ')', 1);
        for (int i = 1; i < end;) {
            readType(methodDesc, &i, end);
            size++;
        }
        auto *parameters = static_cast<CallParameter *>(malloc(size * sizeof(CallParameter)));

        memset(parameters, 0, size * sizeof(CallParameter));

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
        jclass ancc = getDeclaredClass(ArgumentsNativeClass$init);

        auto anc = jni_env->NewObject(
                ancc,
                ArgumentsNativeClass$init
        );

        jobject thisObj;
        if ((modifiers & JVM_ACC_STATIC) == 0) {
            jvmti_env->GetLocalInstance(thread, 0, &thisObj);
        } else {
            thisObj = null;
        }

        jni_env->SetIntField(anc, ArgumentsNativeClass$size, size);
        jni_env->SetLongField(anc, ArgumentsNativeClass$address, (jlong) (&parameters));
        if (thisObj != null) {
            jni_env->SetObjectField(anc, ArgumentsNativeClass$thisObj, thisObj);
        }

        CallParameter returnForce = CallParameter();
        returnForce.edited = false;

        auto forceERN = newForceEarlyReturnNative(jni_env, &returnForce);

        auto inf = new_MethodInfoImpl(jni_env, jvmti_env, methodOwner, &method, metName0, metDesc0, modifiers);
        auto call = jni_env->NewObject(
                c,
                methodCallImplInit,
                methodOwner, metName0, metDesc0,
                modifiers, anc,
                forceERN,
                inf
        );
        auto caller = findCaller(jvmti_env, jni_env, thread);
        jni_env->SetObjectField(call, methodCallImpl$caller, caller);

        jni_env->CallStaticVoidMethod(
                bridgeClass, broadcastMethodEnter, call,
                (jlong) (hookInstances),
                (jint) hookCount
        );

        clearForceEarlyReturnNativeAddress(jni_env, forceERN);
        release_MethodInfoImpl(jni_env, inf);
        jni_env->SetLongField(anc, ArgumentsNativeClass$address, (jlong) 0);

        // allow onMethodExit process
        invokingInfo->hooks = hookInstances;

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

    }

    invokingInfo->runningHook = false;


}

#pragma clang diagnostic pop

JNICALL void onMethodExit(jvmtiEnv *jvmti_env,
                          JNIEnv *jni_env,
                          jthread thread,
                          jmethodID method,
                          jboolean was_popped_by_exception,
                          jvalue return_value) {
    auto invokingInfo = getHookInvokingInfo(jvmti_env, thread);
    // if (invokingInfo->runningHook) return;
    if (invokingInfo->hooks == null) return;
    if (*((void **) (invokingInfo->targetMethod)) != *((void **) method)) return;
    if (invokingInfo->exitingMethod) {
#ifdef DEBUG

        jclass methodDeclaredClass;
        jvmti_env->GetMethodDeclaringClass(method, &methodDeclaredClass);
        char *methodDesc, *methodName, *methodClassName;
        jvmti_env->GetMethodName(method, &methodName, &methodDesc, null);
        jvmti_env->GetClassSignature(methodDeclaredClass, &methodClassName, null);
        jint modifiers;
        jvmti_env->GetMethodModifiers(method, &modifiers);

        std::cout << "[JvmHookFramework - onMethodExit   ] Skipped "
                  << methodClassName << "." << methodName << methodDesc
                  << " because exiting method."
                  << std::endl;
#endif
        return;
    }
    invokingInfo->exitingMethod = true;

    jclass methodDeclaredClass;
    jobject metClassloader;
    jvmti_env->GetMethodDeclaringClass(method, &methodDeclaredClass);
    jvmti_env->GetClassLoader(methodDeclaredClass, &metClassloader);


#ifdef DEBUG
    {
        char *methodDesc, *methodName, *methodClassName;
        jvmti_env->GetMethodName(method, &methodName, &methodDesc, null);
        jvmti_env->GetClassSignature(methodDeclaredClass, &methodClassName, null);
        jint modifiers;
        jvmti_env->GetMethodModifiers(method, &modifiers);

        std::cout << "[JvmHookFramework - onMethodExit   ] Broadcast "
                  << methodClassName << "." << methodName << methodDesc
                  << std::endl;
    }
#endif

    jclass bridgeClass;
    jvmti_env->GetMethodDeclaringClass(broadcastMethodEnter, &bridgeClass);

    auto methodNameJ = invokingInfo->methodName;
    auto methodDescJ = invokingInfo->methodDesc;

    CallParameter returnForce = CallParameter();
    auto cc = newForceEarlyReturnNative(jni_env, &returnForce);
    jint modifiers = invokingInfo->modifiers;

    auto retV = new_JvmReturnValueImpl(jni_env, jvmti_env, was_popped_by_exception, null, &return_value);
    auto metInfo = new_MethodInfoImpl(
            jni_env, jvmti_env, methodDeclaredClass, &method, methodNameJ, methodDescJ,
            modifiers
    );
    auto caller = findCaller(jvmti_env, jni_env, thread);

    jni_env->CallStaticVoidMethod(
            bridgeClass, broadcastMethodExit,
            methodDeclaredClass, methodNameJ, methodDescJ, modifiers, cc, retV,
            metInfo, caller,
            (jlong) invokingInfo->hooks, (jint) invokingInfo->hookCounts
    );
    release_JvmReturnValueImpl(jni_env, retV);
    release_MethodInfoImpl(jni_env, metInfo);


    if (returnForce.edited) {
        jni_env->ExceptionClear();
        char *methodDesc;
        jvmti_env->GetMethodName(method, null, &methodDesc, null);

        switch (methodDesc[indexOf(methodDesc, ')', 0) + 1]) {
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

    free(invokingInfo->hooks);
    invokingInfo->hooks = null;

    release_JvmReturnValueImpl(jni_env, retV);
    clearForceEarlyReturnNativeAddress(jni_env, cc);
    invokingInfo->exitingMethod = false;
}

JNICALL void onThreadDeath(
        jvmtiEnv *jvmti_env,
        JNIEnv *jni_env,
        jthread thread) {

    HookInvokingInfo *info;
    jvmti_env->GetThreadLocalStorage(thread, reinterpret_cast<void **>(&info));
    if (info != null) {
        free(info);
    }
}

JNICALL void onClassPrepare(
        jvmtiEnv *jvmti_env,
        JNIEnv *jni_env,
        jthread thread,
        jclass klass
) {
    auto hookinfo = getHookInvokingInfo(jvmti_env, thread);

    if (hookinfo->broadcastingMethodPrepareLoad || hookinfo->runningHook)
        return;

    jobject klassLoader;
    jvmti_env->GetClassLoader(klass, &klassLoader);
    if (klassLoader == null)return;
    if (jni_env->IsSameObject(klassLoader, classLoaderX)) return;
    if (jni_env->IsSameObject(klassLoader, extClassLoader)) return;
    hookinfo->broadcastingMethodPrepareLoad = true;

    jclass bridgeClass;
    jtiEnv->GetMethodDeclaringClass(broadcastClassPrepare, &bridgeClass);
    jni_env->CallStaticVoidMethod(bridgeClass, broadcastClassPrepare, klass, klassLoader);

#ifdef DEBUG
    {
        char *name;
        jvmti_env->GetClassSignature(klass, &name, null);
        std::cout << "[JvmHookFramework - onClassPrepare ] " << name << std::endl;
    }
#endif

    hookinfo->broadcastingMethodPrepareLoad = false;
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
    memset(&capabilities, 0, sizeof(jvmtiCapabilities));
    // jtiEnv->GetCapabilities(&capabilities);
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
    callbacks.ThreadEnd = onThreadDeath;
    callbacks.ClassPrepare = onClassPrepare;

    if (jtiEnv->SetEventCallbacks(&callbacks, sizeof callbacks) != JNI_OK) {
        std::cerr << "Set callbacks failed" << std::endl;
        return JVMTI_ERROR_INTERNAL;
    }
    jtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_END, null);

    return JVMTI_ERROR_NONE;

}

// region Java Native bridges

JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_initializeNative(JNIEnv *env, jclass owner) {
    //std::cout << "initializeNative called" << std::endl;

    if (jtiEnv == null) {
        env->GetJavaVM(&javaVm);
        Agent_OnLoad(javaVm, null, null);
    }
    jtiEnv->GetClassLoader(owner, &classLoaderX);

    classLoaderX = env->NewGlobalRef(classLoaderX);


    //
    {
        broadcastMethodEnter = env->GetStaticMethodID(
                owner, "broadcastMethodEnter",
                "(Lio/github/karlatemp/jvmhook/call/MethodCall;JI)V"
        );
        broadcastMethodExit = env->GetStaticMethodID(
                owner, "broadcastMethodExit",
                "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/String;ILio/github/karlatemp/jvmhook/call/MethodCall$ForceEarlyReturn;Lio/github/karlatemp/jvmhook/call/MethodReturnValue;Lio/github/karlatemp/jvmhook/call/MethodInfo;Ljava/lang/Class;JI)V"
        );
        broadcastClassPrepare = env->GetStaticMethodID(
                owner, "broadcastClassPrepare",
                "(Ljava/lang/Class;Ljava/lang/ClassLoader;)V"
        );

        jclass MethodCallImpl = env->FindClass("io/github/karlatemp/jvmhook/core/MethodCallImpl");
        methodCallImplInit = env->GetMethodID(
                MethodCallImpl, "<init>",
                "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/String;ILio/github/karlatemp/jvmhook/call/Arguments;Lio/github/karlatemp/jvmhook/call/MethodCall$ForceEarlyReturn;Lio/github/karlatemp/jvmhook/call/MethodInfo;)V"
        );
        methodCallImpl$caller = env->GetFieldID(
                MethodCallImpl, "caller", "Ljava/lang/Class;"
        );

    }
    init_ArgumentNative(env, jtiEnv);
    init_ForceEarlyReturnNative(env, jtiEnv);
    init_JvmReturnValueImpl(env, jtiEnv);
    init_MethodInfoImpl(env);
}

JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_initializeExts(
        JNIEnv *env, jclass
) {
    jtiEnv->SetEventNotificationMode(
            JVMTI_ENABLE,
            JVMTI_EVENT_CLASS_PREPARE,
            null /* all threads */);
    {
        auto e = env->ExceptionOccurred();
        auto extCl = env->FindClass("io/github/karlatemp/jvmhook/core/extsys/ExtLoader");
        if (extCl == null) {
            env->ExceptionClear();
            if (e != null) env->Throw(e);
        } else {
            auto loadMet = env->GetStaticMethodID(extCl, "load", "()V");
            env->CallStaticVoidMethod(extCl, loadMet);
        }
    }

    jtiEnv->SetEventNotificationMode(
            JVMTI_ENABLE,
            JVMTI_EVENT_METHOD_ENTRY,
            null /* all threads */);
    jtiEnv->SetEventNotificationMode(
            JVMTI_ENABLE,
            JVMTI_EVENT_METHOD_EXIT,
            null /* all threads */);
}

JNIEXPORT jobject JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_valueOfPointer
        (JNIEnv *, jclass, jlong pointer, jint index) {
    return (((jobject *) (void *) pointer))[(int) index];
}

// endregion

// region registerHooks

JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_registerHook0(
        JNIEnv *env, jclass,
        jobject method, jobject hook
) {
    jclass k;
    jmethodID met = env->FromReflectedMethod(method);
    jtiEnv->GetMethodDeclaringClass(met, &k);
    registerHook(env, k, met, hook);
}

JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_unregisterHook0(
        JNIEnv *env, jclass, jobject method, jobject hook
) {
    jclass k;
    jmethodID met = env->FromReflectedMethod(method);
    jtiEnv->GetMethodDeclaringClass(met, &k);
    unregisterHook(env, k, met, hook);
}

jmethodID findMetOfHook(JNIEnv *env, jclass target, jstring metName, jstring metDesc) {
    auto metN = env->GetStringUTFChars(metName, null);
    auto metD = env->GetStringUTFChars(metDesc, null);
    auto mi = env->GetMethodID(target, metN, metD);
    if (mi != null) return mi;
    auto mx = env->GetStaticMethodID(target, metN, metD);
    if (mx != null) env->ExceptionClear();
    return mx;
}

JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_registerHook
        (JNIEnv *env, jclass, jclass target, jstring metName, jstring metDesc, jobject hook) {
    auto m = findMetOfHook(env, target, metName, metDesc);
    if (m == null)return;
    registerHook(env, target, m, hook);

    std::cout << "Register hook "
              << env->GetStringUTFChars(metName, null)
              << " - "
              << env->GetStringUTFChars(metDesc, null)
              << " . " << (*((void **) m)) << std::endl;
}

JNIEXPORT void JNICALL Java_io_github_karlatemp_jvmhook_core_Bootstrap_unregisterHook
        (JNIEnv *env, jclass, jclass target, jstring metName, jstring metDesc, jobject hook) {
    auto m = findMetOfHook(env, target, metName, metDesc);
    if (m == null)return;
    unregisterHook(env, target, m, hook);
}
// endregion
