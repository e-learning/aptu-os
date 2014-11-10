import java.io.IOException;
import java.io.PrintWriter;

/**
 * @author adkozlov
 */
public class Task2 {

    private static final String OUTPUT_FORMAT = "%d | %d; %d";

    public static void main(String[] args) {
        try (PrintWriter writer = new PrintWriter("TASK2_DATA")) {
            for (long n = 2; n <= TaskUtils.N_MAX; n += TaskUtils.getIncrement(n)) {
                SieveUniThread uniThread = new SieveUniThread(false, n);
                uniThread.start();

                SieveMultiThread multiThread = new SieveMultiThread(false, n, 1);
                multiThread.start();

                writer.println(String.format(OUTPUT_FORMAT, uniThread.getRunTime(), multiThread.getRunTime(), n));
            }
        } catch (IOException e) {
            System.err.println(e);
        }
    }
}
