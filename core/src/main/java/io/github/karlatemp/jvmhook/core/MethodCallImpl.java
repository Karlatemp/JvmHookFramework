package io.github.karlatemp.jvmhook.core;

import io.github.karlatemp.jvmhook.call.Arguments;
import io.github.karlatemp.jvmhook.call.MethodCall;
import io.github.karlatemp.jvmhook.call.MethodInfo;

class MethodCallImpl implements MethodCall {
    private final Class<?> owner;
    private final String metName;
    private final String metDesc;
    private final int modifiers;
    private final Arguments args;
    private final ForceEarlyReturn fer;
    private final MethodInfo mi;

    MethodCallImpl(
            Class<?> owner,
            String metName,
            String metDesc,
            int modifiers,
            Arguments args,
            ForceEarlyReturn fer,
            MethodInfo mi
    ) {
        this.owner = owner;
        this.metName = metName;
        this.metDesc = metDesc;
        this.modifiers = modifiers;
        this.args = args;
        this.fer = fer;
        this.mi = mi;
        // Thread.dumpStack();
    }

    @Override
    public Class<?> getMethodOwner() {
        return owner;
    }

    @Override
    public String getMethodName() {
        return metName;
    }

    @Override
    public String getMethodDescription() {
        return metDesc;
    }

    @Override
    public int getMethodModifiers() {
        return modifiers;
    }

    @Override
    public Arguments arguments() {
        return args;
    }

    @Override
    public ForceEarlyReturn earlyReturn() {
        return fer;
    }

    @Override
    public MethodInfo methodInfo() {
        return mi;
    }
}
