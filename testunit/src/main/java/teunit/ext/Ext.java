package teunit.ext;

import io.github.karlatemp.jvmhook.JvmHookFramework;
import tui.TestRun;

public class Ext {
    private static void load() {
        System.out.println("Loaded");
        JvmHookFramework instance = JvmHookFramework.INSTANCE;
        instance.registerHook(TestRun.class, "error", "()V",
                call -> call.earlyReturn().returnVoid()
        );
        instance.registerHook(TestRun.class, "ret", "()Ljava/lang/String;",
                call -> call.earlyReturn().returnObject("B")
        );
        instance.registerHook(TestRun.class, "argChange", "(Ljava/lang/String;)V", call -> {
            call.arguments().putObject(0, "X");
        });
        instance.registerHook(TestRun.class, "main", "([Ljava/lang/String;)V", call -> Thread.dumpStack());
    }
}
