package io.github.karlatemp.jvmhook.call;

public interface JvmValue {
    int asInt();

    byte asByte();

    boolean asBoolean();

    char asChar();

    short asShort();

    long asLong();

    float asFloat();

    double asDouble();

    Object asObject();
}
