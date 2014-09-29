import java.io.*;
import java.io.IOException;
import java.util.*;

/**
 * @author adkozlov
 */
public class Scheduler implements Runnable {

    public static final String FORMAT_STRING = "%d %s\n";
    public static final String IDLE_STRING = "IDLE";

    private static class Operation implements Comparable<Operation> {

        private long startTimestamp;
        private final long duration;

        private Operation(long startTimestamp, long duration) {
            this.startTimestamp = startTimestamp;
            this.duration = duration;
        }

        public long getStartTimestamp() {
            return startTimestamp;
        }

        public void setStartTimestamp(long startTimestamp) {
            this.startTimestamp = startTimestamp;
        }

        public long getDuration() {
            return duration;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (!(o instanceof Operation)) return false;

            Operation operation = (Operation) o;

            if (duration != operation.duration) return false;
            if (startTimestamp != operation.startTimestamp) return false;

            return true;
        }

        @Override
        public int hashCode() {
            int result = (int) (startTimestamp ^ (startTimestamp >>> 32));
            result = 31 * result + (int) (duration ^ (duration >>> 32));
            return result;
        }

        @Override
        public int compareTo(Operation o) {
            return Long.compare(o.startTimestamp, startTimestamp);
        }
    }

    private class Process implements Comparable<Process> {

        private final String id;
        private final Operation operation;
        private final Queue<Operation> ioOperations;

        private long timePassed = 0;
        private boolean quantumUsed = false;

        private Process(String id, Operation operation) {
            this.id = id;
            this.operation = operation;
            ioOperations = new PriorityQueue<>();
        }

        public void setStartTimestamp(long startTimestamp) {
            operation.setStartTimestamp(startTimestamp);
        }

        public long getTimePassed() {
            return timePassed;
        }

        public void setTimePassed(long timePassed) {
            this.timePassed = timePassed;
        }

        public void setQuantumUsed(boolean quantumUsed) {
            this.quantumUsed = quantumUsed;
        }

        public boolean addIOOperation(Operation o) {
            return ioOperations.add(o);
        }

        public long getActivationTime() {
            return operation.startTimestamp;
        }

        public long getDuration() {
            return operation.getDuration();
        }

        public boolean isAvailableAt(long time) {
            return time >= operation.startTimestamp;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (!(o instanceof Process)) return false;

            Process process = (Process) o;

            if (timePassed != process.timePassed) return false;
            if (id != null ? !id.equals(process.id) : process.id != null) return false;
            if (ioOperations != null ? !ioOperations.equals(process.ioOperations) : process.ioOperations != null)
                return false;
            if (operation != null ? !operation.equals(process.operation) : process.operation != null) return false;

            return true;
        }

        @Override
        public int hashCode() {
            int result = id != null ? id.hashCode() : 0;
            result = 31 * result + (operation != null ? operation.hashCode() : 0);
            result = 31 * result + (ioOperations != null ? ioOperations.hashCode() : 0);
            result = 31 * result + (int) (timePassed ^ (timePassed >>> 32));
            return result;
        }

        @Override
        public int compareTo(Process p) {
            return Long.compare(priority(), p.priority());
        }

        private long fromBoolean(boolean value) {
            return value ? 1 : 0;
        }

        private long priority() {
            return  7 * (quantum - (getDuration() - timePassed)) + 5 * fromBoolean(hasIO()) + 3 * fromBoolean(quantumUsed);
        }

        public Operation peekIO() {
            return ioOperations.peek();
        }

        public Operation pollIO() {
            return ioOperations.poll();
        }

        public boolean hasIO() {
            return !ioOperations.isEmpty() && peekIO().startTimestamp <= timePassed + quantum;
        }

        public boolean isTerminated() {
            return getDuration() <= timePassed;
        }
    }

    private BufferedReader br;
    private PrintWriter pw;

    private long time = 0;
    private long quantum;

    @Override
    public void run() {
        try {
            br = new BufferedReader(new InputStreamReader(System.in));
            pw = new PrintWriter(System.out);

            quantum = nextLong(new StringTokenizer(br.readLine()));

            Queue<Process> queue = readProcesses();
            while (!queue.isEmpty()) {
                List<Process> available = new ArrayList<>();

                for (Iterator<Process> i = queue.iterator(); i.hasNext(); ) {
                    Process p = i.next();

                    if (p.isAvailableAt(time)) {
                        available.add(p);
                        i.remove();
                    }
                }

                if (available.isEmpty()) {
                    pw.printf(FORMAT_STRING, time, IDLE_STRING);
                    time = queue.peek().getActivationTime();
                } else {
                    Collections.sort(available);
                    Process current = available.remove(available.size() - 1);
                    queue.addAll(available);

                    pw.printf(FORMAT_STRING, time, current.id);

                    if (current.hasIO()) {
                        Operation o = current.pollIO();

                        long timePassed = o.getStartTimestamp() - current.getTimePassed();
                        time += timePassed;

                        current.setStartTimestamp(time + o.getDuration());
                        current.setTimePassed(timePassed + o.getDuration());
                        current.setQuantumUsed(timePassed < quantum);

                        queue.add(current);
                    } else {
                        long timePassed = Math.min(quantum, current.getDuration() - current.getTimePassed());
                        time += timePassed;

                        current.setTimePassed(current.getTimePassed() + timePassed);

                        if (!current.isTerminated()) {
                            current.setStartTimestamp(time);
                            current.setQuantumUsed(true);

                            queue.add(current);
                        }
                    }
                }
            }

            pw.close();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(1);
        }
    }

    private Queue<Process> readProcesses() throws IOException {
        Queue<Process> result = new PriorityQueue<>();

        for (String line = br.readLine(); line != null && !line.isEmpty(); line = br.readLine()) {
            StringTokenizer st = new StringTokenizer(line);

            Process process = new Process(next(st), new Operation(nextLong(st), nextLong(st)));
            while (st.hasMoreTokens()) {
                process.addIOOperation(new Operation(nextLong(st), nextLong(st)));
            }

            result.add(process);
        }

        return result;
    }

    private static String next(StringTokenizer st) throws IOException {
        return st.nextToken();
    }

    private static long nextLong(StringTokenizer st) throws IOException {
        return Long.parseLong(next(st));
    }

    public static void main(String[] args) throws IOException {
        new Thread(new Scheduler()).run();
    }
}
