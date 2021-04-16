package io.github.karlatemp.jvmhook.core;

import io.github.karlatemp.jvmhook.call.MethodInfo;

import java.lang.reflect.Method;

class MethodInfoImpl implements MethodInfo {
    private final String desc;
    private final String n;
    private final Class<?> c;
    private final int modi;
    private long pointer;

    MethodInfoImpl(Class<?> c, String n, String desc, int modi) {
        this.c = c;
        this.n = n;
        this.desc = desc;
        this.modi = modi;
    }

    @Override
    public Class<?> getDeclaredClass() {
        return c;
    }

    @Override
    public String getMethodName() {
        return n;
    }

    @Override
    public String getMethodDescription() {
        return desc;
    }

    @Override
    public int getMethodModifiers() {
        return modi;
    }

    @Override
    public native Method toMethod();
}
