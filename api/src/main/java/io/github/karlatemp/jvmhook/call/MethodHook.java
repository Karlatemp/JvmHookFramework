package io.github.karlatemp.jvmhook.call;

public interface MethodHook {
    void preInvoke(MethodCall call) throws Throwable;
}
