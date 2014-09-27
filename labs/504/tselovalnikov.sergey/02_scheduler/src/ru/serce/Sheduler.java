package ru.serce;

import java.util.Comparator;
import java.util.PriorityQueue;
import java.util.Scanner;

/**
 * @author Sergey Tselovalnikov
 * @since 27.09.14
 */
public class Sheduler {
    private int quantum;
    private int time;
    private Task idleTask = new Task("IDLE", 0, 0);

    private PriorityQueue<Task> readyTasks = new PriorityQueue<Task>(10, new Comparator<Task>() {
        @Override
        public int compare(Task t1, Task t2) {
            return t1.remainingTime() - t2.remainingTime();
        }
    });

    private PriorityQueue<Task> waitingTasks = new PriorityQueue<Task>(10, new Comparator<Task>() {
        @Override
        public int compare(Task t1, Task t2) {
            return t1.getStartTime() - t2.getStartTime();
        }
    });

    public static void main(String[] args) {
        new Sheduler().run();
    }

    public void run() {
        fill();
        while (!waitingTasks.isEmpty() || !readyTasks.isEmpty()) {
            handleTasks();
        }
    }

    private void handleTasks() {
        while (!waitingTasks.isEmpty() && waitingTasks.peek().getStartTime() <= time) {
            readyTasks.add(waitingTasks.poll());
        }

        if (readyTasks.isEmpty()) {
            execTask(idleTask);
            time = waitingTasks.peek().getStartTime();
            return;
        }

        Task currentTask = readyTasks.poll();
        execTask(currentTask);

        int runtime = Math.min(quantum, currentTask.remainingTime());
        currentTask.increaseRunnedTime(runtime);
        time += runtime;

        if (currentTask.remainingTime() > 0) {
            readyTasks.add(currentTask);
            return;
        }

        if (!currentTask.getIoOperations().isEmpty()) {
            int ioLength = currentTask.getIoOperations().poll().getEnd();
            currentTask.increaseRunnedTime(ioLength);
            currentTask.setStartTime(time + ioLength);
            waitingTasks.add(currentTask);
        }
    }

    private void execTask(Task currentTask) {
        System.out.println(time + " " + currentTask.getName());
    }

    private void fill() {
        Scanner scanner = new Scanner(System.in);
        quantum = Integer.valueOf(scanner.nextLine());
        while (scanner.hasNextLine()) {
            String line = scanner.nextLine();
            if (line.isEmpty()) {
                return;
            }
            Task task = Task.parseLine(line);
            waitingTasks.add(task);
        }
    }
}
