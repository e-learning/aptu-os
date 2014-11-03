package pavljuchenko.denis;

import java.io.Serializable;

public class Block implements Serializable {
    private int id;
    private int size;

    public Block(int i, int s)
    {
        if (s < 1024)
        {
            throw new IllegalArgumentException("size of block < 1024");
        }

        this.size = s;
        this.id = i;
    }

    public int getId() {
        return id;
    }

    public int getSize() {
        return size;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Block)) return false;

        Block block = (Block) o;

        if (id != block.id) return false;

        return true;
    }

    @Override
    public int hashCode() {
        return id;
    }
}
