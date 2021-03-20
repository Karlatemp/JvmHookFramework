package tui;

public class TestRun {
    public static void error() {
        throw new Error();
    }

    public static String ret() {
        return "A";
    }

    public static void argChange(String arg) {
        if (arg.equals("V")) throw new AssertionError();
    }

    public static void main(String[] args) throws Throwable {
        error();
        if ("A".equals(ret())) {
            throw new AssertionError();
        }
        argChange("V");
        System.out.println("Completed");
    }
}
