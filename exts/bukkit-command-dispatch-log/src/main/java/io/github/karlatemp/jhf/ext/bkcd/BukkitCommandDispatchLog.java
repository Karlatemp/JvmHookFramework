package io.github.karlatemp.jhf.ext.bkcd;

import io.github.karlatemp.jvmhook.JvmHookFramework;
import io.github.karlatemp.unsafeaccessor.UnsafeAccess;

import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodType;
import java.util.function.BiConsumer;

class BukkitCommandDispatchLog implements BiConsumer<Class<?>, ClassLoader> {
    static final JvmHookFramework instance = JvmHookFramework.INSTANCE;
    static final UnsafeAccess UA = UnsafeAccess.getInstance();
    static final String TARGET = "org.bukkit.Server";

    private static void load() {
        instance.registerClassPrepareHook(new BukkitCommandDispatchLog());
    }

    @Override
    public void accept(Class<?> klass, ClassLoader classLoader) {
        tigger:
        {
            for (Class<?> interface0 : klass.getInterfaces()) {
                if (interface0.getName().equals(TARGET)) break tigger;
            }
            return;
        }
        instance.unregisterClassPrepareHook(this);
        //boolean dispatchCommand(CommandSender sender, String commandLine)
        MethodHandle CS$name;
        try {
            Class<?> CS = Class.forName("org.bukkit.command.CommandSender", false, classLoader);
            CS$name = UA.getTrustedIn(CS).findVirtual(CS, "getName", MethodType.methodType(String.class));
        } catch (Exception e) {
            throw new RuntimeException(e);
        }

        instance.registerHook(klass, "dispatchCommand", "(Lorg/bukkit/command/CommandSender;Ljava/lang/String;)Z", call -> {
            try {
                Object executor = call.arguments().getAsObject(0);
                System.out.println("" + CS$name.invoke(executor) + " executed command `" + call.arguments().getAsObject(1) + "` by `Server.dispatchCommand`");
            } catch (Throwable e) {
                UA.getUnsafe().throwException(e);
            }
        });

    }
}
