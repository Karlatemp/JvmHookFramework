# 下载

点击 [Actions](https://github.com/Karlatemp/JvmHookFramework/actions).
点击最后一个成功运行的 Action.

拖到最下面, 在 Artifacts 中点击 `Jars` 和 相应平台名字 (如 `windows-shared-x64`)

# 运行

下载之后, 应该有如下文件
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

你需要按照如下的格式修改你的 java 命令行来使用 JvmHookFramework

```shell
java -agentpath:/path/to/libnative.so -javaagent:/path/to/jvm-hook-framework-launcher.jar ...
# Or
java -agentpath:/path/to/libnative.so -javaagent:/path/to/jvm-hook-framework-launcher-obf.jar ...
```

# 按照扩展

在运行一次 java 命令之后, 一个名为 `jvm-hook-framework-extensions` 会在运行目录创建.
把扩展放进这个文件夹里

你也可以改成其他的位置. 只需要设置 环境变量 `JVM_HOOK_FRAMEWORK_EXTENSIONS`.
```shell
#!/usr/bin/env bash

JVM_HOOK_FRAMEWORK_EXTENSIONS=/path/to/other/dir
java ....
```

# 开发扩展

[![Maven Central](https://img.shields.io/maven-central/v/io.github.karlatemp.jvm-hook-framework/api.svg?label=api%20on%20Maven%20Central)](https://search.maven.org/search?q=io.github.karlatemp.jvm-hook-framework)

添加依赖 `io.github.karlatemp.jvm-hook-framework:api` (最新版本)

使用 `io.github.karlatemp.jvmhook.JvmHookFramework` 注册挂钩

构建的 jar 里需要存在一个名为 `jvm-hook-ext.txt` 且内容为扩展主类的全名称 的文件

示例: [TestExtension](testunit/src/main/java/teunit/ext/Ext.java)
