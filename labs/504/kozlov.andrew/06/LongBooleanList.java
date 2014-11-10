import java.util.Arrays;

public class LongBooleanList {

    private static long MAX_ARRAY_LENGTH = Long.MAX_VALUE;
    private static int MAX_ROW_LENGTH = Integer.MAX_VALUE >> 2;

    private long size;
    private boolean[][] array;

    public LongBooleanList(long size, boolean defaultValue) {
        checkIndex(size);

        this.size = size;
        array = allocateArray(size);

        for (boolean[] row : array) {
            Arrays.fill(row, defaultValue);
        }
    }

    public synchronized long getSize() {
        return size;
    }

    public boolean get(long index) {
        checkIndex(index);

        return array[getRow(index)][getId(index)];
    }

    public void set(long index, boolean value) {
        checkIndex(index);

        array[getRow(index)][getId(index)] = value;
    }

    private static void checkIndex(long index) {
        if (index > MAX_ARRAY_LENGTH) {
            throw new IllegalArgumentException("Array size/index should be an integer number less than " + MAX_ARRAY_LENGTH);
        }
    }

    private static int getRow(long index) {
        return (int) (index / MAX_ROW_LENGTH);
    }

    private static int getId(long index) {
        return (int) (index % MAX_ROW_LENGTH);
    }

    private static boolean[][] allocateArray(long length) {
        int size = getRow(length) + 1;
        boolean[][] result = new boolean[size][];

        for (int i = 0; i < size - 1; i++) {
            result[i] = new boolean[MAX_ROW_LENGTH];
        }

        result[size - 1] = new boolean[getId(length)];

        return result;
    }
}
