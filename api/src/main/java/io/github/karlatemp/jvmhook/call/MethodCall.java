package io.github.karlatemp.jvmhook.call;

import java.lang.invoke.MethodHandles;

public interface MethodCall {
    Class<?> getMethodOwner();


    MethodHandles.Lookup getMethodOwnerLookup();

    Class<?> getMethodCaller();

    String getMethodName();

    String getMethodDescription();

    int getMethodModifiers();

    Arguments arguments();

    ForceEarlyReturn earlyReturn();

    MethodInfo methodInfo();

    public interface ForceEarlyReturn {
        void returnVoid();

        void returnInt(int value);

        void returnFloat(float value);

        void returnDouble(double value);

        void returnLong(long value);

        void returnShort(short value);

        void returnChar(char value);

        void returnByte(byte value);

        void returnBoolean(boolean value);

        void returnObject(Object value);
    }
}
