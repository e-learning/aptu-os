/**
 * @author adkozlov
 */
public class TaskUtils {

    public static final int N_MAX = 100000000;
    public static final int M_MAX = 20;
    public static final int RUNS_COUNT = 5;

    public static long getIncrement(long i) {
        int log = (int) Math.log10(i);
        return (long) Math.pow(10, log);
    }
}
