import java.util.Comparator;
import java.util.List;
import java.util.PriorityQueue;

public class Scheduler {

    private final int quantum;

    private final PriorityQueue<Task> notAvailableTasks = new PriorityQueue<>(15, new NotAvailableTasksComparator());
    private final PriorityQueue<StartedTask> startedTasks = new PriorityQueue<>(15, new StartedTasksComparator());
    private final PriorityQueue<StartedIOTask> startedIOTasks = new PriorityQueue<>(15, new StartedIOTasksComparator());

    private int time = 0;

    public Scheduler(int quantum) {
        this.quantum = quantum;
    }

    public void calculate(List<Task> tasks) {
        for (Task task : tasks) {
            if (task.availabilityTime <= time) {
                startedTasks.add(new StartedTask(task));
            } else {
                notAvailableTasks.add(task);
            }
        }

        while (!startedTasks.isEmpty() || !notAvailableTasks.isEmpty() || !startedIOTasks.isEmpty()) {
            while (!notAvailableTasks.isEmpty() && notAvailableTasks.peek().availabilityTime <= time) {
                startedTasks.add(
                        new StartedTask(
                                notAvailableTasks.poll()
                        )
                );
            }

            while (!startedIOTasks.isEmpty() && startedIOTasks.peek().endTime <= time) {
                startedTasks.add(
                        startedIOTasks.poll().startedTask
                );
            }

            if (startedTasks.isEmpty()) {
                System.out.println(time + " " + "IDLE");

                if (notAvailableTasks.isEmpty()) {
                    time = startedIOTasks.peek().endTime;
                } else if (startedIOTasks.isEmpty()) {
                    time = notAvailableTasks.peek().availabilityTime;
                } else {
                    time = Math.min(notAvailableTasks.peek().availabilityTime, startedIOTasks.peek().endTime);
                }

                continue;
            }

            StartedTask current = startedTasks.poll();

            System.out.println(time + " " + current.task.id);

            int allocatedDuration = Math.min(quantum, current.getDesiredDuration());
            int workingDuration = current.run(allocatedDuration);

            if (workingDuration != allocatedDuration) {
                startedIOTasks.add(
                        new StartedIOTask(
                                current, time + workingDuration
                        )
                );
            } else {
                if (current.getDesiredDuration() > 0) {
                    startedTasks.add(current);
                }
            }

            time += allocatedDuration;
        }
    }

    private static class StartedTask {

        public final Task task;

        private int elapsedTime = 0;
        private int nextIOTask = 0;

        public StartedTask(Task task) {
            this.task = task;
        }

        public int getDesiredDuration() {
            if (!hasNextIOTask()) {
                return task.duration - elapsedTime;
            } else {
                return task.ioTasks.get(nextIOTask).startTime - elapsedTime;
            }
        }

        public int run(int allocatedDuration) {
            int workingDuration = calculateWorkingDuration(allocatedDuration);

            elapsedTime += workingDuration;

            return workingDuration;
        }

        private boolean hasNextIOTask() {
            return nextIOTask != task.ioTasks.size();
        }

        private int calculateWorkingDuration(int allocatedDuration) {
            if (!hasNextIOTask() || task.ioTasks.get(nextIOTask).startTime != elapsedTime + allocatedDuration) {
                return allocatedDuration;
            } else {
                nextIOTask++;
                return allocatedDuration + task.ioTasks.get(nextIOTask - 1).duration;
            }
        }
    }

    public static class StartedIOTask {

        public final StartedTask startedTask;
        public final int endTime;

        public StartedIOTask(StartedTask startedTask, int endTime) {
            this.startedTask = startedTask;
            this.endTime = endTime;
        }
    }

    private static class NotAvailableTasksComparator implements Comparator<Task> {

        @Override
        public int compare(Task a, Task b) {
            return a.availabilityTime - b.availabilityTime;
        }
    }

    private static class StartedTasksComparator implements Comparator<StartedTask> {

        @Override
        public int compare(StartedTask a, StartedTask b) {
            return a.getDesiredDuration() - b.getDesiredDuration();
        }
    }

    private static class StartedIOTasksComparator implements Comparator<StartedIOTask> {

        @Override
        public int compare(StartedIOTask o1, StartedIOTask o2) {
            return o1.endTime - o2.endTime;
        }
    }
}