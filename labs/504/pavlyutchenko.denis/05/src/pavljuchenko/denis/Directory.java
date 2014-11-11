package pavljuchenko.denis;

import java.io.*;
import java.util.ArrayList;
import java.util.HashSet;

public class Directory implements IFile, Serializable {
    private String name;
    private Directory parentDirectory = null;
    private final ArrayList<IFile> files = new ArrayList<>();
    private final ArrayList<Block> blocks = new ArrayList<>();
    private String fullPath;

    private Directory() {}

    public Directory(String name, Directory parent) {
        if (name == null || name.isEmpty())
        {
            throw new NullPointerException("name");
        }

        if (parent == null)
        {
            throw new NullPointerException("parent");
        }

        this.name = name;
        this.parentDirectory = parent;

        this.parentDirectory.appendFile(this);

        String basePath = parent.getFullPath();

        if (!basePath.equals(java.io.File.separator))
        {
            basePath += java.io.File.separator;
        }

        fullPath = basePath + getName();
    }

    public void appendFile(IFile iFile)
    {
        if (iFile == null)
        {
            throw new NullPointerException("iFile");
        }
        files.add(iFile);
    }

    public void removeFile(IFile iFile)
    {
        if (iFile == null)
        {
            throw new NullPointerException("iFile");
        }

        int index = 0;
        Boolean isFind = false;

        for (IFile f : files)
        {
            if (f.getFullPath().equals(iFile.getFullPath()))
            {
                isFind = true;
                break;
            }
            index++;
        }

        if (isFind)
        {
            files.remove(index);
        }
    }

    public ArrayList<IFile> getFiles()
    {
        return files;
    }

    static public Directory CreateRootDirectory() {
        Directory r = new Directory();

        r.name = "" + java.io.File.separator;

        r.fullPath = r.name;

        return r;
    }

    @Override
    public String getFullPath()
    {
        return fullPath;
    }

    @Override
    public ArrayList<Block> getBlocks() {
        return blocks;
    }

    @Override
    public Directory getParentDirectory() {
        return parentDirectory;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public Boolean isDirectory() {
        return true;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Directory)) return false;

        Directory directory = (Directory) o;

        if (!fullPath.equals(directory.fullPath)) return false;

        return true;
    }

    @Override
    public int hashCode() {
        return fullPath.hashCode();
    }
}
