package io.github.karlatemp.jvmhook;

import io.github.karlatemp.jvmhook.call.MethodHook;
import org.jetbrains.annotations.NotNull;

import java.lang.annotation.Native;
import java.lang.reflect.Method;
import java.util.function.BiConsumer;

public abstract class JvmHookFramework {

    @SuppressWarnings("ConstantConditions")
    @Native
    public static final @NotNull JvmHookFramework INSTANCE = NIL();

    @SuppressWarnings("ConstantConditions")
    @Native
    public static final @NotNull JvmLowLevelAccess LOW_LEVEL_ACCESS = NIL();

    static <T> T NIL() {
        return null;
    }

    public abstract void registerHook(
            Class<?> target, String methodName, String methodDesc, MethodHook hook
    );

    public abstract void unregisterHook(
            Class<?> target, String methodName, String methodDesc, MethodHook hook
    );

    public abstract void registerClassPrepareHook(
            BiConsumer<Class<?>, ClassLoader> hook
    );

    public abstract void unregisterClassPrepareHook(
            BiConsumer<Class<?>, ClassLoader> hook
    );

    protected abstract String getMethodDesc(Method method);

    public void registerHook(Method method, MethodHook hook) {
        registerHook(method.getDeclaringClass(), method.getName(), getMethodDesc(method), hook);
    }

    public void unregisterHook(Method method, MethodHook hook) {
        unregisterHook(method.getDeclaringClass(), method.getName(), getMethodDesc(method), hook);
    }
}
