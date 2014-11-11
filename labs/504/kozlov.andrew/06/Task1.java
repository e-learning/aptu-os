import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

/**
 * @author adkozlov
 */
public class Task1 {

    private static final String OUTPUT_FORMAT = "%d; %d; %d";

    public static void main(String[] args) {
        List<Long> numbers = new ArrayList<>();
        for (long n = 2; n <= TaskUtils.N_MAX; n += TaskUtils.getIncrement(n)) {
            numbers.add(n);
        }

        try (PrintWriter writer = new PrintWriter("TASK1_DATA")) {
            for (int m = 1; m <= TaskUtils.M_MAX; m++) {
                for (long n : numbers) {
                    AbstractSieve sieve = m == 1 ? new SieveUniThread(false, n) : new SieveMultiThread(false, n, m);

                    sieve.start();
                    writer.println(String.format(OUTPUT_FORMAT, m, n, sieve.getRunTime()));
                }
            }
        } catch (IOException e) {
            System.err.println(e);
        }
    }
}
