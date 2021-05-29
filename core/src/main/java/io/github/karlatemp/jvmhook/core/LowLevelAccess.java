package io.github.karlatemp.jvmhook.core;

import io.github.karlatemp.jvmhook.JvmLowLevelAccess;

import java.lang.invoke.MethodHandles;
import java.security.ProtectionDomain;

class LowLevelAccess extends JvmLowLevelAccess {
    @Override
    public void setLookupClass(MethodHandles.Lookup lookup, Class<?> lookupClass) {
        Bootstrap.changeOwner(lookup, lookupClass);
    }

    @Override
    public native ClassLoader getClassLoader(Class<?> klass);

    @Override
    public void throw0(Throwable throwable) {
        Bootstrap.unsafe.throwException(throwable);
    }

    @Override
    public Class<?> defineClass(ClassLoader loader, String name, byte[] code, int offset, int length, ProtectionDomain protectionDomain) {
        return Bootstrap.unsafe.defineClass(name, code, offset, length, loader, protectionDomain);
    }
}
