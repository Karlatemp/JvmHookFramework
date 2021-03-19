package io.github.karlatemp.jvmhook.call;

public interface Arguments {
    int size();

    int getAsInt(int index);

    byte getAsByte(int index);

    double getAsDouble(int index);

    float getAsFloat(int index);

    short getAsShort(int index);

    boolean getAsBoolean(int index);

    long getAsLong(int index);

    char getAsChar(int index);

    Object getAsObject(int index);

    Object getThis();

    void putInt(int index, int value);

    void putByte(int index, byte value);

    void putFloat(int index, float value);

    void putShort(int index, short value);

    void putBoolean(int index, boolean value);

    void putDouble(int index, double value);

    void putLong(int index, long value);

    void putChar(int index, char value);

    void putObject(int index, Object value);
}
