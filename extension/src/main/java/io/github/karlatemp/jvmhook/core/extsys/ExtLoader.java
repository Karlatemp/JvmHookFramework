package io.github.karlatemp.jvmhook.core.extsys;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.net.URL;
import java.net.URLClassLoader;
import java.nio.charset.StandardCharsets;
import java.security.AllPermission;
import java.security.CodeSource;
import java.security.PermissionCollection;
import java.util.ArrayList;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class ExtLoader {
    public static void load() throws Throwable {
        String exts = "jvm-hook-framework-extensions";
        boolean userset = false;
        String ENV = System.getenv("JVM_HOOK_FRAMEWORK_EXTENSIONS");
        if (ENV != null) {
            userset = true;
            exts = ENV;
        } else {
            String property = System.getProperty("jvm-hook-framework-extensions");
            if (property != null) {
                exts = property;
                userset = true;
            }
        }
        File dir = new File(exts);
        if (!userset) {
            dir.mkdirs();
        }
        File[] jrs = dir.listFiles();
        List<URL> urls = new ArrayList<>();
        List<File> jars = new ArrayList<>();
        if (jrs != null) {
            for (File f : jrs) {
                if (f.isFile() && f.getName().endsWith(".jar")) {
                    urls.add(f.toURI().toURL());
                    jars.add(f);
                }
            }
        }
        URLClassLoader ucl = new URLClassLoader(
                urls.toArray(new URL[0]),
                ExtLoader.class.getClassLoader()
        ) {
            @Override
            protected PermissionCollection getPermissions(CodeSource codesource) {
                AllPermission ap = new AllPermission();
                PermissionCollection collection = ap.newPermissionCollection();
                collection.add(ap);
                collection.setReadOnly();
                return collection;
            }
        };
        NativeBridge.setExtClassLoader(ucl);
        for (File jar : jars) {
            try (ZipFile zip = new ZipFile(jar)) {
                ZipEntry entry = zip.getEntry("jvm-hook-ext.txt");
                if (entry == null) continue;
                try (BufferedReader br = new BufferedReader(new InputStreamReader(
                        zip.getInputStream(entry), StandardCharsets.UTF_8
                ))) {
                    String line;
                    while (true) {
                        line = br.readLine();
                        if (line == null) break;
                        line = line.trim();
                        if (line.isEmpty()) continue;
                        if (line.charAt(0) == '#') continue;
                        NativeBridge.invokeLoad(Class.forName(line, false, ucl));
                    }
                }
            }
        }
    }
}
