package io.github.karlatemp.jvmhook;

import io.github.karlatemp.jvmhook.call.MethodHook;
import org.jetbrains.annotations.NotNull;

import java.lang.annotation.Native;

public abstract class JvmHookFramework {

    @SuppressWarnings("ConstantConditions")
    @Native
    public static final @NotNull JvmHookFramework INSTANCE = NIL();

    static JvmHookFramework NIL() {
        return null;
    }

    public abstract void registerHook(
            Class<?> target, String methodName, String methodDesc, MethodHook hook
    );

    public abstract void unregisterHook(
            Class<?> target, String methodName, String methodDesc, MethodHook hook
    );

}
