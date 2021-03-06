# JvmHookFramework
HookFramework for jvm

See (KasukuSakura/JvmHookFramework)[https://github.com/KasukuSakura/JvmHookFramework]

[简体中文](README-zh.md)

**Warning: Not suitable for production environment, very slow**

# Download

Go to [Actions](https://github.com/Karlatemp/JvmHookFramework/actions).
Client the latest successful CI run.

Move to bottom, you will see artifacts. Download `Jars` and `OS library`

# Run

After downloaded. Your disk has the following files:
```text
Jars.zip
    `- api-1.0.0.jar                        -  The api for developers
    `- jvm-hook-framework-core.jar          -  Runtime java library (shadowed)
    `- jvm-hook-framework-launcher.jar      -  JHF Launcher (Javaagent)
    `- jvm-hook-framework-launcher-obf.jar  -  JHF Launcher (Javaagent) (Relocated)
    `- jvm-hook-framework-obf.jar           -  JHF Launcher (Javaagent) (Relocated) (Not runnable)
NativeLib.zip
    `- libnative.dll / libnative.so         - native lib (native agent)
```

You need change your java command line to use JvmHookFramework. Like:
```shell
java -agentpath:/path/to/libnative.so -javaagent:/path/to/jvm-hook-framework-launcher.jar ...
# Or
java -agentpath:/path/to/libnative.so -javaagent:/path/to/jvm-hook-framework-launcher-obf.jar ...
```

# Install extensions

Run java once. A folder named `jvm-hook-framework-extensions` will created in working direction.
Place extensions in it.

You can change it to other locations. Just setup `JVM_HOOK_FRAMEWORK_EXTENSIONS` environment.
```shell
#!/usr/bin/env bash

JVM_HOOK_FRAMEWORK_EXTENSIONS=/path/to/other/dir
java ....
```

# Develop extension

[![Maven Central](https://img.shields.io/maven-central/v/io.github.karlatemp.jvm-hook-framework/api.svg?label=api%20on%20Maven%20Central)](https://search.maven.org/search?q=io.github.karlatemp.jvm-hook-framework)

Depend on the latest version of `io.github.karlatemp.jvm-hook-framework:api`

Use `io.github.karlatemp.jvmhook.JvmHookFramework` to register hooks

Built jar need contains a file named `jvm-hook-ext.txt` that contains ExtMainClass.

Example extension: [TestExtension](testunit/src/main/java/teunit/ext/Ext.java)
