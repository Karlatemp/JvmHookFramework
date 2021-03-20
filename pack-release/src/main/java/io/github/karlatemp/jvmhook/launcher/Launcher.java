package io.github.karlatemp.jvmhook.launcher;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class Launcher {
    public static void premain(String args) throws Throwable {
        Map<String, byte[]> resources = new HashMap<>();
        ByteArrayOutputStream bos = new ByteArrayOutputStream(1024);
        byte[] buffer = new byte[1024];
        try (ZipInputStream zip = new ZipInputStream(new BufferedInputStream(
                Launcher.class.getResourceAsStream("classes.jar")
        ))) {
            ZipEntry entry;
            while (true) {
                entry = zip.getNextEntry();
                if (entry == null) break;
                if (entry.isDirectory()) continue;
                bos.reset();
                while (true) {
                    int len = zip.read(buffer);
                    if (len == -1) break;
                    bos.write(buffer, 0, len);
                }

                String n = entry.getName();
                if (!n.isEmpty()) {
                    if (n.charAt(0) == '/') n = n.substring(1);
                }
                resources.put(n, bos.toByteArray());
            }
        }
        RamClassLoader cl = new RamClassLoader(resources, Launcher.class.getClassLoader());
        Class.forName("io.github.karlatemp.jvmhook.core.Bootstrap", true, cl);
    }
}
