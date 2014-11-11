public class SieveUniThread extends AbstractSieve {

    public SieveUniThread(boolean printFlag, long n) {
        super(printFlag, n);
    }

    @Override
    protected void run() {
        for (long i = 2; !isFinished(i); i++) {
            markNotPrimes(i);
        }
    }

    public static void main(String[] args) {
        printUsage(args);
        SieveUniThread sieve = parseArgs(args);

        sieve.start();
        sieve.printIfNeeded();
    }

    private static void printUsage(String[] args) {
        if (args.length < 1 || args.length > 2) {
            System.out.println("Usage: [-p] <n>");
            System.exit(1);
        }
    }

    private static SieveUniThread parseArgs(String[] args) {
        boolean print = args.length == 2;
        int shift = print ? 1 : 0;

        return new SieveUniThread(print, Long.parseLong(args[shift]));
    }
}
