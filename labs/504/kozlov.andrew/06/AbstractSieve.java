public abstract class AbstractSieve {

    private final long n;
    private final LongBooleanList sieve;
    private final boolean printFlag;

    private long runTime;

    public AbstractSieve(boolean printFlag, long n) {
        this.n = n;
        sieve = new LongBooleanList(n, true);
        this.printFlag = printFlag;

        runTime = -1;
    }

    public final long getRunTime() {
        return runTime;
    }

    protected final void start() {
        long start = System.currentTimeMillis();
        run();

        runTime = System.currentTimeMillis() - start;
    }

    protected abstract void run();

    protected final void markNotPrimes(long i) {
        if (sieve.get(i)) {
            for (long j = i; i * j < n; j++) {
                sieve.set(i * j, false);
            }
        }
    }

    protected final boolean isFinished(long i) {
        return i * i >= n;
    }

    public final void printIfNeeded() {
        if (printFlag) {
            for (long i = 2; i < sieve.getSize(); i++) {
                if (sieve.get(i)) {
                    System.out.println(i);
                }
            }
        }
    }
}
