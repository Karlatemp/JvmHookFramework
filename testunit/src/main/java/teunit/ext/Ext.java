package teunit.ext;

import io.github.karlatemp.jvmhook.JvmHookFramework;
import io.github.karlatemp.jvmhook.call.MethodCall;
import io.github.karlatemp.jvmhook.call.MethodHook;
import io.github.karlatemp.jvmhook.call.MethodReturnValue;
import tui.TestRun;

public class Ext {
    static void dumpStack() {
        new Exception("Stack trace").printStackTrace(System.out);
    }

    static void dumpStack(String prefix) {
        new Exception("Stack trace: " + prefix).printStackTrace(System.out);
    }

    private static void load() throws Throwable {
        System.setErr(System.out);
        dumpStack("Loaded TestExt");
        System.out.println("MethodHook:         " + MethodHook.class.getClassLoader());
        JvmHookFramework instance = JvmHookFramework.INSTANCE;
        System.out.println("JvmHookFramework:   " + instance.getClass().getClassLoader());
        System.out.println("F JVML:             " + JvmHookFramework.LOW_LEVEL_ACCESS.getClassLoader(Ext.class));
        instance.registerHook(TestRun.class, "error", "()V",
                call -> call.earlyReturn().returnVoid()
        );
        instance.registerHook(TestRun.class, "ret", "()Ljava/lang/String;",
                call -> call.earlyReturn().returnObject("B")
        );
        instance.registerHook(TestRun.class, "argChange", "(Ljava/lang/String;)V", call -> {
            call.arguments().putObject(0, "X");
        });
        instance.registerHook(TestRun.class, "main", "([Ljava/lang/String;)V", call -> dumpStack());
        instance.registerHook(TestRun.class, "postMethodEdit", "()I", new MethodHook() {
            @Override
            public void preInvoke(MethodCall call) throws Throwable {
            }

            @Override
            public void postInvoke(MethodCall call, MethodReturnValue returnValue) throws Throwable {
                call.earlyReturn().returnInt(50);
                System.out.println("TT Return is " + returnValue.asInt());
                dumpStack();
            }
        });
        instance.registerHook(TestRun.class, "preThrowError", "()V", call -> {
            throw new Exception();
        });
        instance.registerHook(TestRun.class, "postThrowError", "()V", new MethodHook() {
            @Override
            public void preInvoke(MethodCall call) throws Throwable {
            }

            @Override
            public void postInvoke(MethodCall call, MethodReturnValue returnValue) throws Throwable {
                throw new Exception();
            }
        });
        instance.registerHook(TestRun.class, "error1", "()V", new MethodHook() {
            @Override
            public void preInvoke(MethodCall call) throws Throwable {
            }

            @Override
            public void postInvoke(MethodCall call, MethodReturnValue returnValue) throws Throwable {
                System.out.println("OR " + returnValue.asObject());
                System.out.println("IsE: " + returnValue.isException());
                System.out.println("Err: " + returnValue.exception());
                call.earlyReturn().returnVoid();
            }
        });
        instance.registerHook(TestRun.class.getMethod("main", String[].class), call -> {
            dumpStack("Call by Reflection register");
            System.out.println(call.methodInfo().toMethod());
            System.out.println("Lookup: " + call.getMethodOwnerLookup());
            if (call.getMethodCaller() != null) {
                throw new AssertionError("Caller not null");
            }
        });
        instance.registerHook(Thread.class.getMethod("dumpStack"), call -> {
            call.earlyReturn().returnVoid();
            dumpStack("Injected Thread dump(caller = " + call.getMethodCaller() + ")");
        });

    }
}
