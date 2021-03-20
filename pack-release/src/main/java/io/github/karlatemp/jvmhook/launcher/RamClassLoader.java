package io.github.karlatemp.jvmhook.launcher;

import org.jetbrains.annotations.Nullable;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.security.AllPermission;
import java.security.PermissionCollection;
import java.security.ProtectionDomain;
import java.util.Map;

public class RamClassLoader extends ClassLoader {
    private final Map<String, byte[]> res;
    private final ProtectionDomain domain;

    public RamClassLoader(Map<String, byte[]> resources, ClassLoader cp) {
        super(cp);
        this.res = resources;
        AllPermission ap = new AllPermission();
        PermissionCollection collection = ap.newPermissionCollection();
        collection.add(ap);
        domain = new ProtectionDomain(null, collection);
    }

    @Nullable
    @Override
    public InputStream getResourceAsStream(String name) {
        InputStream is = super.getResourceAsStream(name);
        if (is == null) {
            byte[] bytes = res.get(name);
            if (bytes != null) return new ByteArrayInputStream(bytes);
        }
        return is;
    }

    @Override
    protected Class<?> findClass(String name) throws ClassNotFoundException {
        byte[] rs = res.get(name.replace('.', '/') + ".class");
        if (rs != null) {
            return defineClass(null, rs, 0, rs.length, domain);
        }
        return super.findClass(name);
    }
}
