package ru.serce;

import java.util.PriorityQueue;

/**
 * @author Sergey Tselovalnikov
 * @since 27.09.14
 */
public class Task {
    private String name;
    private int startTime;
    private int time;
    private int runnedTime = 0;
    private PriorityQueue<IOOperation> ioOperations = new PriorityQueue<IOOperation>();

    public Task(String name, int startTime, int time) {
        this.name = name;
        this.startTime = startTime;
        this.time = time;
    }

    public static Task parseLine(String line) {
        String[] args = line.split(" ");
        Task task = new Task(args[0], Integer.valueOf(args[1]), Integer.valueOf(args[2]));
        for (int i = 3; i < args.length; i += 2) {
            IOOperation op = new IOOperation(Integer.valueOf(args[i]), Integer.valueOf(args[i + 1]));
            task.addIOOpertation(op);
        }
        return task;
    }

    public void increaseRunnedTime(int time) {
        this.runnedTime += time;
    }

    public PriorityQueue<IOOperation> getIoOperations() {
        return ioOperations;
    }

    public int remainingTime() {
        if (ioOperations.isEmpty()) {
            return time - runnedTime;
        } else {
            return ioOperations.peek().getStart() - runnedTime;
        }
    }

    public String getName() {
        return name;
    }

    public int getStartTime() {
        return startTime;
    }

    public void setStartTime(int startTime) {
        this.startTime = startTime;
    }

    public void addIOOpertation(IOOperation op) {
        ioOperations.add(op);
    }
}
