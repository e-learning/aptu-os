package ru.serce;

/**
 * @author Sergey Tselovalnikov
 * @since 27.09.14
 */
public class IOOperation implements Comparable<IOOperation> {
    private int start;
    private int end;

    public IOOperation(int start, int end) {
        this.start = start;
        this.end = end;
    }

    public int getStart() {
        return start;
    }

    public int getEnd() {
        return end;
    }

    @Override
    public String toString() {
        return "IOOperation{" +
                "start=" + start +
                ", end=" + end +
                '}';
    }

    @Override
    public int compareTo(IOOperation other) {
        return start - other.start;
    }
}
