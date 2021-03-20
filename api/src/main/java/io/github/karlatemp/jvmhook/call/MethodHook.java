package io.github.karlatemp.jvmhook.call;

public interface MethodHook {
    void preInvoke(MethodCall call) throws Throwable;

    default void postInvoke(MethodCall call, MethodReturnValue returnValue) throws Throwable {
    }
}
