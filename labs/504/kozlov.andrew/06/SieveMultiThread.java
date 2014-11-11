import java.util.concurrent.atomic.AtomicLong;

public class SieveMultiThread extends AbstractSieve {

    private final AtomicLong current;
    private final Thread[] threads;

    public SieveMultiThread(boolean printFlag, long n, int m) {
        super(printFlag, n);

        current = new AtomicLong(2);
        threads = new Thread[m];

        for (int i = 0; i < m; i++) {
            threads[i] = new Thread(new SieveRunnable());
        }
    }

    @Override
    protected void run() {
        for (Thread thread : threads) {
            thread.start();

            try {
                thread.join();
            } catch (InterruptedException e) {
                System.err.println(e.getMessage());
            }
        }
    }

    public static void main(String[] args) {
        printUsage(args);
        SieveMultiThread sieve = parseArgs(args);

        sieve.start();
        sieve.printIfNeeded();
    }

    private static void printUsage(String[] args) {
        if (args.length < 2 || args.length > 3) {
            System.out.println("Usage: [-p] <n> <m>");
            System.exit(1);
        }
    }

    private static SieveMultiThread parseArgs(String[] args) {
        boolean print = args.length == 3;
        int shift = print ? 1 : 0;

        return new SieveMultiThread(print, Long.parseLong(args[shift]), Integer.parseInt(args[shift + 1]));
    }

    private class SieveRunnable implements Runnable {

        @Override
        public void run() {
            while (true) {
                long i = current.getAndIncrement();
                if (isFinished(i)) {
                    return;
                }

                markNotPrimes(i);
            }
        }
    }
}
