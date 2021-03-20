package io.github.karlatemp.jvmhook.core.extsys;

class NativeBridge {
    static native void setExtClassLoader(ClassLoader loader);

    static native void invokeLoad(Class<?> c) throws Throwable;
}
