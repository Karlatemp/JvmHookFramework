package io.github.karlatemp.jvmhook.core;

import io.github.karlatemp.jvmhook.call.Arguments;
import io.github.karlatemp.jvmhook.call.MethodCall;
import io.github.karlatemp.jvmhook.call.MethodInfo;

import java.lang.annotation.Native;
import java.lang.invoke.MethodHandles;

class MethodCallImpl implements MethodCall {
    private final Class<?> owner;
    private final String metName;
    private final String metDesc;
    private final int modifiers;
    private final Arguments args;
    private final ForceEarlyReturn fer;
    private final MethodInfo mi;
    @Native
    Class<?> caller = null;

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
    public MethodHandles.Lookup getMethodOwnerLookup() {
        MethodHandles.Lookup lk = MethodHandles.lookup();
        Bootstrap.changeOwner(lk, owner);
        return lk;
    }

    @SuppressWarnings("ConstantConditions")
    @Override
    public Class<?> getMethodCaller() {
        return caller;
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
