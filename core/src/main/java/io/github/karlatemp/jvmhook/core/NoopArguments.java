package io.github.karlatemp.jvmhook.core;

import io.github.karlatemp.jvmhook.call.Arguments;

class NoopArguments implements Arguments {
    static final NoopArguments I = new NoopArguments();

    @Override
    public int size() {
        return 0;
    }

    @Override
    public int getAsInt(int index) {
        return 0;
    }

    @Override
    public byte getAsByte(int index) {
        return 0;
    }

    @Override
    public double getAsDouble(int index) {
        return 0;
    }

    @Override
    public float getAsFloat(int index) {
        return 0;
    }

    @Override
    public short getAsShort(int index) {
        return 0;
    }

    @Override
    public boolean getAsBoolean(int index) {
        return false;
    }

    @Override
    public long getAsLong(int index) {
        return 0;
    }

    @Override
    public char getAsChar(int index) {
        return 0;
    }

    @Override
    public Object getAsObject(int index) {
        return null;
    }

    @Override
    public Object getThis() {
        return null;
    }

    @Override
    public void putInt(int index, int value) {

    }

    @Override
    public void putByte(int index, byte value) {

    }

    @Override
    public void putFloat(int index, float value) {

    }

    @Override
    public void putShort(int index, short value) {

    }

    @Override
    public void putBoolean(int index, boolean value) {

    }

    @Override
    public void putDouble(int index, double value) {

    }

    @Override
    public void putLong(int index, long value) {

    }

    @Override
    public void putChar(int index, char value) {

    }

    @Override
    public void putObject(int index, Object value) {

    }
}
