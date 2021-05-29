package mwcs;

public class ServerImpl implements Server {
    @Override
    public void dispatchCommand(String command) {
        throw new AssertionError(command);
    }
}
