package iovs;

public class Tst {
    public Tst() {
        test(90.4187);
        testInt(90);
        nope("Test");
    }

    public static void test(double value) {
        System.out.println("Value is : " + value);
    }

    public static void testInt(int value) {
        System.out.println("Value is : " + value);
    }

    public static void nope(String argument) {
        System.err.println("!!!!!!!! " + argument);
    }
}
