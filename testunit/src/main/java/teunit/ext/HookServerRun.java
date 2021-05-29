package teunit.ext;

import io.github.karlatemp.jvmhook.JvmHookFramework;

import java.util.function.BiConsumer;

class HookServerRun {
    private static void load() {
        JvmHookFramework framework = JvmHookFramework.INSTANCE;

        framework.registerClassPrepareHook(new BiConsumer<Class<?>, ClassLoader>() {
            @Override
            public void accept(Class<?> klass, ClassLoader classLoader) {
                System.out.println("Cl Loading " + klass);
                boolean runIt = false;
                for (Class<?> c : klass.getInterfaces()) {
                    System.out.println(klass.getName() + " - " + c.getName());
                    if (c.getName().equals("mwcs.Server")) {
                        runIt = true;
                        break;
                    }
                }
                if (!runIt) return;
                System.out.println("Do it! " + klass);
                framework.registerHook(klass, "dispatchCommand", "(Ljava/lang/String;)V", call -> {
                    call.earlyReturn().returnVoid();
                    System.out.println("[Server] Instance:       " + call.arguments().getThis());
                    System.out.println("[Server] Command invoke: " + call.arguments().getAsObject(0));
                });
                framework.unregisterClassPrepareHook(this);
            }
        });
    }
}
