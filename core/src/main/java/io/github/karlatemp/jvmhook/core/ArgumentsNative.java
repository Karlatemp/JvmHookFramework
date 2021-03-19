package io.github.karlatemp.jvmhook.core;

import io.github.karlatemp.jvmhook.call.Arguments;

class ArgumentsNative implements Arguments {
    int size;
    long address;
    Object thisObj;

    @Override
    public int size() {
        return size;
    }

    @Override
    public native int getAsInt(int index);

    @Override
    public native byte getAsByte(int index);

    @Override
    public native double getAsDouble(int index);

    @Override
    public native float getAsFloat(int index);

    @Override
    public native short getAsShort(int index);

    @Override
    public native boolean getAsBoolean(int index);

    @Override
    public native long getAsLong(int index);

    @Override
    public native char getAsChar(int index);

    @Override
    public native Object getAsObject(int index);

    @Override
    public Object getThis() {
        return thisObj;
    }

    @Override
    public native void putInt(int index, int value);

    @Override
    public native void putByte(int index, byte value);

    @Override
    public native void putFloat(int index, float value);

    @Override
    public native void putShort(int index, short value);

    @Override
    public native void putBoolean(int index, boolean value);

    @Override
    public native void putDouble(int index, double value);

    @Override
    public native void putLong(int index, long value);

    @Override
    public native void putChar(int index, char value);

    @Override
    public native void putObject(int index, Object value);
}
