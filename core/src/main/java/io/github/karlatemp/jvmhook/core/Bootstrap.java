package io.github.karlatemp.jvmhook.core;

import io.github.karlatemp.jvmhook.JvmHookFramework;
import io.github.karlatemp.jvmhook.call.MethodCall;
import io.github.karlatemp.jvmhook.call.MethodHook;
import io.github.karlatemp.jvmhook.call.MethodInfo;
import io.github.karlatemp.jvmhook.call.MethodReturnValue;
import io.github.karlatemp.unsafeaccessor.Unsafe;

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
    static final WeakHashMap<Class<?>, Map<String, Map<String, Queue<MethodHook>>>> hooks = new WeakHashMap<>();
    static final ConcurrentLinkedDeque<BiConsumer<Class<?>, ClassLoader>> classPrepareLoadingHook = new ConcurrentLinkedDeque<>();

    static {
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
            Bootstrap.registerHook(target, methodName, methodDesc, hook);
        }

        @Override
        public void unregisterHook(Class<?> target, String methodName, String methodDesc, MethodHook hook) {
            Bootstrap.unregisterHook(target, methodName, methodDesc, hook);
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
            unsafe.ensureClassInitialized(JvmHookFramework.class);
            unsafe.putReference(
                    unsafe.staticFieldBase(f),
                    unsafe.staticFieldOffset(f),
                    new JvmHookFrameworkBridge()
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

    public static void registerHook(
            Class<?> target, String methodName, String methodDesc, MethodHook hook
    ) {
        synchronized (Bootstrap.class) {
            notifyHookClass(target);
            registerHook0(
                    ("L" + target.getName() + ";").replace('.', '/'),
                    methodName,
                    methodDesc
            );
        }
        Map<String, Map<String, Queue<MethodHook>>> tables = getTables(target, true);
        synchronized (tables) {
            tables.computeIfAbsent(
                    methodName, $ -> new HashMap<>()
            ).computeIfAbsent(methodDesc, $ -> new ConcurrentLinkedDeque<>())
                    .add(hook);
        }
    }

    public static void unregisterHook(
            Class<?> target, String methodName, String methodDesc, MethodHook hook
    ) {
        synchronized (Bootstrap.class) {
            unregisterHook0(
                    ("L" + target.getName() + ";").replace('.', '/'),
                    methodName,
                    methodDesc
            );
        }
        Map<String, Map<String, Queue<MethodHook>>> tables = getTables(target, false);
        if (tables == null) return;
        synchronized (tables) {
            Map<String, Queue<MethodHook>> queueMap = tables.get(methodName);
            if (queueMap == null) return;
            Queue<MethodHook> hooks = queueMap.get(methodDesc);
            if (hooks != null) {
                hooks.remove(hook);
            }
        }
    }

    private static native void registerHook0(
            String className, String methodName, String methodDesc
    );

    private static native void unregisterHook0(
            String className, String methodName, String methodDesc
    );

    private static boolean shouldHookMethodEnter(
            Class<?> klass,
            ClassLoader classLoader,
            String name,
            String desc,
            int modifiers
    ) {
        if (klass.getName().startsWith("jdk.internal.reflect.")) return false;
        if (klass.getName().startsWith("sun.reflect.")) return false;
        Map<String, Map<String, Queue<MethodHook>>> tables = getTables(klass, false);
        if (tables == null) return false;
        Map<String, Queue<MethodHook>> queueMap = tables.get(name);
        if (queueMap == null) return false;
        Queue<MethodHook> hooks = queueMap.get(desc);
        return hooks != null && !hooks.isEmpty();
    }

    private static void broadcastMethodEnter(
            MethodCall call
    ) throws Throwable {

        Map<String, Map<String, Queue<MethodHook>>> tables = getTables(call.getMethodOwner(), false);
        if (tables == null) return;
        Map<String, Queue<MethodHook>> queueMap = tables.get(call.getMethodName());
        if (queueMap == null) return;
        Queue<MethodHook> hooks = queueMap.get(call.getMethodDescription());
        if (hooks != null) {
            for (MethodHook hook : hooks) {
                hook.preInvoke(call);
            }
        }
    }

    public static void premain(Instrumentation instrumentation) throws Throwable {
    }

    private static void broadcastMethodExit(
            Class<?> c, String n, String desc, int modifier,
            MethodCall.ForceEarlyReturn fer,
            MethodReturnValue mrv,
            MethodInfo mi,
            Class<?> caller
    ) throws Throwable {

        Map<String, Map<String, Queue<MethodHook>>> tables = getTables(c, false);
        if (tables == null) return;
        Map<String, Queue<MethodHook>> queueMap = tables.get(n);
        if (queueMap == null) return;
        Queue<MethodHook> hooks = queueMap.get(desc);
        if (hooks != null) {
            MethodCallImpl mc = new MethodCallImpl(
                    c, n, desc, modifier, NoopArguments.I, fer, mi
            );
            mc.caller = caller;
            for (MethodHook hook : hooks) {
                hook.postInvoke(mc, mrv);
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
