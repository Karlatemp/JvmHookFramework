package io.github.karlatemp.jvmhook.core;

import io.github.karlatemp.jvmhook.call.MethodReturnValue;

class JvmReturnValueImpl implements MethodReturnValue {
    private final Throwable e;
    private final boolean isE;
    private long address;

    JvmReturnValueImpl(Throwable e, boolean isE) {
        this.e = e;
        this.isE = isE;
    }

    @Override
    public native int asInt();

    @Override
    public native byte asByte();

    @Override
    public native boolean asBoolean();

    @Override
    public native char asChar();

    @Override
    public native short asShort();

    @Override
    public native long asLong();

    @Override
    public native float asFloat();

    @Override
    public native double asDouble();

    @Override
    public native Object asObject();

    @Override
    public boolean isException() {
        return isE;
    }

    @Override
    public Throwable exception() {
        return e;
    }
}
