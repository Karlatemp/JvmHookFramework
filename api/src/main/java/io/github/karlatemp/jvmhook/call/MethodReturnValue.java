package io.github.karlatemp.jvmhook.call;

public interface MethodReturnValue extends JvmValue {
    boolean isException();

    Throwable exception();
}
