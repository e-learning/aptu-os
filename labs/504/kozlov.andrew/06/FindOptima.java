import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * @author adkozlov
 */
public class FindOptima {

    private static final String OUTPUT_FORMAT = "optima: m = %d";

    public static void main(String[] args) {
        List<Pair> runTimes = new ArrayList<>();
        for (int m = 2; m <= TaskUtils.M_MAX; m++) {
            long average = 0;

            for (int i = 0; i < TaskUtils.RUNS_COUNT; i++) {
                SieveMultiThread sieve = new SieveMultiThread(false, TaskUtils.N_MAX, m);
                sieve.start();

                average += sieve.getRunTime();
            }

            average /= TaskUtils.RUNS_COUNT;

            Pair pair = new Pair(m, average);
            runTimes.add(pair);
            System.out.println(pair);
        }

        System.out.println(String.format(OUTPUT_FORMAT, Collections.min(runTimes).getM()));
    }

    private static class Pair implements Comparable<Pair> {
        private final int m;
        private final long runTime;

        public Pair(int m, long runTime) {
            this.m = m;
            this.runTime = runTime;
        }

        @Override
        public int compareTo(Pair o) {
            return Long.compare(runTime, o.runTime);
        }

        public int getM() {
            return m;
        }

        @Override
        public String toString() {
            return "m = " + m + ", runTime = " + runTime;
        }
    }
}
