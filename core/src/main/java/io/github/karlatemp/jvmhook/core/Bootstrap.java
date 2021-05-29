package io.github.karlatemp.jvmhook.core;

import io.github.karlatemp.jvmhook.JvmHookFramework;
import io.github.karlatemp.jvmhook.call.MethodCall;
import io.github.karlatemp.jvmhook.call.MethodHook;
import io.github.karlatemp.jvmhook.call.MethodInfo;
import io.github.karlatemp.jvmhook.call.MethodReturnValue;
import io.github.karlatemp.unsafeaccessor.Unsafe;
import io.github.karlatemp.unsafeaccessor.UnsafeAccess;

import java.lang.instrument.Instrumentation;
import java.lang.invoke.MethodHandles;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;
import java.util.Queue;
import java.util.WeakHashMap;
import java.util.concurrent.ConcurrentLinkedDeque;
import java.util.function.BiConsumer;

public class Bootstrap {
    static final Unsafe unsafe;
    static final UnsafeAccess unsafeAccess;
    static final WeakHashMap<Class<?>, Map<String, Map<String, Queue<MethodHook>>>> hooks = new WeakHashMap<>();
    static final ConcurrentLinkedDeque<BiConsumer<Class<?>, ClassLoader>> classPrepareLoadingHook = new ConcurrentLinkedDeque<>();

    static {
        unsafeAccess = UnsafeAccess.getInstance();
        unsafe = Unsafe.getUnsafe();
        initializeBridge();
        initializeNative();
        {
            MethodHandles.Lookup lk = MethodHandles.lookup();
            initLookupFieldOffset(lk, lk.lookupClass());
        }
        initializeExts();
    }

    private static native void initializeNative();

    private static native void initializeExts();

    private static class JvmHookFrameworkBridge extends JvmHookFramework {
        @Override
        public void registerHook(Class<?> target, String methodName, String methodDesc, MethodHook hook) {
            synchronized (this) {
                Bootstrap.registerHook(target, methodName, methodDesc, hook);
            }
        }

        @Override
        public void unregisterHook(Class<?> target, String methodName, String methodDesc, MethodHook hook) {
            synchronized (this) {
                Bootstrap.unregisterHook(target, methodName, methodDesc, hook);
            }
        }

        @Override
        public void registerHook(Method method, MethodHook hook) {
            synchronized (this) {
                Bootstrap.registerHook0(method, hook);
            }
        }

        @Override
        public void unregisterHook(Method method, MethodHook hook) {
            synchronized (this) {
                Bootstrap.unregisterHook0(method, hook);
            }
        }

        @Override
        protected native String getMethodDesc(Method method);

        @Override
        public void registerClassPrepareHook(BiConsumer<Class<?>, ClassLoader> hook) {
            classPrepareLoadingHook.add(hook);
        }

        @Override
        public void unregisterClassPrepareHook(BiConsumer<Class<?>, ClassLoader> hook) {
            classPrepareLoadingHook.remove(hook);
        }
    }

    private static void initializeBridge() {
        try {
            Field f = JvmHookFramework.class.getDeclaredField("INSTANCE");
            Field f2 = JvmHookFramework.class.getDeclaredField("LOW_LEVEL_ACCESS");
            unsafe.ensureClassInitialized(JvmHookFramework.class);
            unsafe.putReference(
                    unsafe.staticFieldBase(f),
                    unsafe.staticFieldOffset(f),
                    new JvmHookFrameworkBridge()
            );
            unsafe.putReference(
                    unsafe.staticFieldBase(f2),
                    unsafe.staticFieldOffset(f2),
                    new LowLevelAccess()
            );
        } catch (Exception e) {
            throw new Error(e);
        }
    }

    private static Map<String, Map<String, Queue<MethodHook>>> getTables(Class<?> c, boolean create) {
        Map<String, Map<String, Queue<MethodHook>>> map = hooks.get(c);
        if (map != null) return map;
        if (create) {
            synchronized (hooks) {
                map = hooks.computeIfAbsent(c, k -> new HashMap<>());
            }
        }
        return map;
    }

    private native static void registerHook(
            Class<?> target, String methodName, String methodDesc, MethodHook hook
    );

    private native static void unregisterHook(
            Class<?> target, String methodName, String methodDesc, MethodHook hook
    );

    private static native void registerHook0(
            Method method, MethodHook hook
    );

    private static native void unregisterHook0(
            Method method, MethodHook hook
    );

    private static native Object valueOfPointer(
            long pointer, int index
    );

    private static void broadcastMethodEnter(
            MethodCall call, long pointer, int size
    ) throws Throwable {
        if (size < 1) return;
        for (int i = 0; i < size; i++) {
            ((MethodHook) valueOfPointer(pointer, i)).preInvoke(call);
        }
    }

    public static void premain(Instrumentation instrumentation) throws Throwable {
    }

    private static void broadcastMethodExit(
            Class<?> c, String n, String desc, int modifier,
            MethodCall.ForceEarlyReturn fer,
            MethodReturnValue mrv,
            MethodInfo mi,
            Class<?> caller,
            long pointer, int size
    ) throws Throwable {
        if (size > 0) {
            MethodCallImpl mc = new MethodCallImpl(
                    c, n, desc, modifier, NoopArguments.I, fer, mi
            );
            mc.caller = caller;
            for (int i = 0; i < size; i++) {
                ((MethodHook) valueOfPointer(pointer, i)).postInvoke(mc, mrv);
            }
        }
    }

    private static void broadcastClassPrepare(Class<?> klass, ClassLoader loader) {
        for (BiConsumer<Class<?>, ClassLoader> hook : classPrepareLoadingHook) {
            hook.accept(klass, loader);
        }
    }

    private static native void notifyHookClass(Class<?> c);

    private static native void initLookupFieldOffset(MethodHandles.Lookup lookup, Class<?> owner);

    /*package-private*/
    static native void changeOwner(MethodHandles.Lookup lookup, Class<?> target);
}
