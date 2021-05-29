package tui;

import mwcs.Server;
import mwcs.ServerImpl;

public class TestRun {
    public static void error() {
        throw new Error();
    }

    public static void error1() {
        throw new Error();
    }

    public static String ret() {
        return "A";
    }

    public static void argChange(String arg) {
        if (arg.equals("V")) throw new AssertionError();
    }

    private static boolean postMethodEditCalled = false;

    public static int postMethodEdit() {
        postMethodEditCalled = true;
        return 20;
    }

    public static void preThrowError() {
    }

    public static void postThrowError() {
    }

    public static void main(String[] args) throws Throwable {
        error();
        if ("A".equals(ret())) {
            throw new AssertionError();
        }
        argChange("V");
        if (postMethodEdit() == 20) {
            throw new AssertionError();
        }
        if (!postMethodEditCalled) throw new AssertionError();

        try {
            preThrowError();
            throw new AssertionError();
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }
        try {
            postThrowError();
            throw new AssertionError();
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }
        error1();
        Thread.dumpStack();

        {
            Server server = new ServerImpl();
            server.dispatchCommand("whoami");
        }

        System.out.println("Completed");
    }
}
