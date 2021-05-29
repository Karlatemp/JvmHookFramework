package io.github.karlatemp.jvmhook;

import java.lang.invoke.MethodHandles;
import java.security.ProtectionDomain;

public abstract class JvmLowLevelAccess {
    public abstract void setLookupClass(
            MethodHandles.Lookup lookup,
            Class<?> lookupClass
    );

    public abstract ClassLoader getClassLoader(Class<?> klass);

    public abstract void throw0(Throwable throwable);

    public abstract Class<?> defineClass(
            ClassLoader loader,
            String name,
            byte[] code,
            int offset,
            int length,
            ProtectionDomain protectionDomain
    );
}
