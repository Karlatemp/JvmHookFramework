package io.github.karlatemp.jvmhook.call;

import java.lang.reflect.Method;

public interface MethodInfo {
    Class<?> getDeclaredClass();

    String getMethodName();

    String getMethodDescription();

    int getMethodModifiers();

    Method toMethod();
}
