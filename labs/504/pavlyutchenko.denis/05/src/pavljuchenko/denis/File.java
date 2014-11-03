package pavljuchenko.denis;

import java.io.Serializable;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Arrays;

public class File implements IFile, Serializable {
    private String name;
    private Directory directory;
    private final ArrayList<Block> blocks = new ArrayList<>();
    private String fullPath;
    private byte[] data;
    private int sizeOfFile;

    public File(String name, Directory directory, byte[] data) {
        if (name == null || name.isEmpty())
        {
            throw new NullPointerException("name");
        }

        if (directory == null)
        {
            throw new NullPointerException("directory");
        }

        if (data.length < 1)
        {
            throw new NullPointerException("data");
        }

        this.data = Arrays.copyOf(data, data.length);

        this.sizeOfFile = data.length;

        this.directory = directory;

        this.name = name;

        String basePath = this.directory.getFullPath();

        if (!basePath.equals(java.io.File.separator))
        {
            basePath += java.io.File.separator;
        }

        this.fullPath = basePath + name;

        this.directory = directory;
        this.directory.appendFile(this);
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public Boolean isDirectory() {
        return false;
    }

    @Override
    public String getFullPath() {
        return this.fullPath;
    }

    @Override
    public ArrayList<Block> getBlocks() {
        return blocks;
    }

    @Override
    public Directory getParentDirectory() {
        return directory;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof File)) return false;

        File file = (File) o;

        if (!fullPath.equals(file.fullPath)) return false;

        return true;
    }

    @Override
    public int hashCode() {
        return fullPath.hashCode();
    }

    public byte[] getData() {
        return data;
    }

    public int getSizeOfFile() {
        return sizeOfFile;
    }

    @Override
    public String toString() {
        return getFullPath() + " " + getBlocks().size();
    }
}
