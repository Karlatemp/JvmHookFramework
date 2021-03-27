package ci;

import kotlin.io.FilesKt;

import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.HashMap;
import java.util.Map;

public class RunCi {
    public static void main(String[] args) throws Throwable {
        File artifacts = new File(args[0]);
        // osname
        //    `- library.dll
        // Jars
        //    `- ....
        String version = args[1];
        String postUrl = args[2];
        postUrl = postUrl.replaceAll("\\{.*\\}", "");
        System.out.println("Post url: " + postUrl);

        String prefix = "jvm-hook-framework-";
        Map<String, File> files = new HashMap<>();
        for (File dir : artifacts.listFiles()) {
            if (dir.getName().equalsIgnoreCase("Jars")) {
                for (File f : dir.listFiles()) {
                    files.put(f.getName(), f);
                }
            } else {
                for (File f : dir.listFiles()) {
                    String name = prefix + dir.getName() + "-" + version + '.' + FilesKt.getExtension(f);
                    files.put(name, f);
                }
            }
        }
        byte[] buf = new byte[20480];


        for (Map.Entry<String, File> fileEntry : files.entrySet()) {
            URL u = new URL(postUrl + "?name=" + URLEncoder.encode(fileEntry.getKey(), "UTF-8"));
            HttpURLConnection connection = (HttpURLConnection) u.openConnection();
            connection.setRequestMethod("POST");
            connection.setRequestProperty("Accept", "application/vnd.github.v3+json");
            connection.setRequestProperty("Content-Length", String.valueOf(fileEntry.getValue().length()));
            connection.setRequestProperty("Content-Type", "application/zip");
            connection.setRequestProperty("Authorization", "token " + System.getenv("GH_TOKEN"));

            connection.setDoOutput(true);
            {
                OutputStream outputStream = connection.getOutputStream();
                try (InputStream is = new BufferedInputStream(new FileInputStream(fileEntry.getValue()))) {
                    while (true) {
                        int len = is.read(buf);
                        if (len == -1) break;
                        outputStream.write(buf, 0, len);
                        outputStream.flush();
                    }
                }
                outputStream.close();
            }

            if (connection.getResponseCode() != 201) {
                int len = 20480;
                try {
                    len = Math.max(len, connection.getHeaderFieldInt("Content-Length", len));
                } catch (Throwable ignore) {
                }
                ByteArrayOutputStream bos = new ByteArrayOutputStream(len);
                InputStream is;
                try {
                    is = connection.getErrorStream();
                } catch (Throwable ignore) {
                    is = connection.getInputStream();
                }
                while (true) {
                    int lenx = is.read(buf);
                    if (lenx == -1) break;
                    bos.write(buf, 0, lenx);
                }
                throw new IOException(bos.toString("UTF-8"));
            }
        }
    }
}
