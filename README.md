# JvmHookFramework
HookFramework for jvm

[简体中文](README-zh.md)

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

Add `Jars.zip > api.jar` into dependencies.
Use `io.github.karlatemp.jvmhook.JvmHookFramework` to register hooks

Built jar need contains a file named `jvm-hook-ext.txt` that contains ExtMainClass.

Example extension: [TestExtension](testunit/src/main/java/teunit/ext/Ext.java)
