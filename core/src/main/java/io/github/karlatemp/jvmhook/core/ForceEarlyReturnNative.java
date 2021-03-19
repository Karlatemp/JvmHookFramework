package io.github.karlatemp.jvmhook.core;

import io.github.karlatemp.jvmhook.call.MethodCall;

class ForceEarlyReturnNative implements MethodCall.ForceEarlyReturn {
    private long address;

    @Override
    public native void returnVoid();

    @Override
    public native void returnInt(int value);

    @Override
    public native void returnFloat(float value);

    @Override
    public native void returnDouble(double value);

    @Override
    public native void returnLong(long value);

    @Override
    public void returnShort(short value) {
        returnInt(value);
    }

    @Override
    public void returnChar(char value) {
        returnInt(value);
    }

    @Override
    public void returnByte(byte value) {
        returnInt(value);
    }

    @Override
    public void returnBoolean(boolean value) {
        returnInt(value ? 1 : 0);
    }

    @Override
    public native void returnObject(Object value);
}
