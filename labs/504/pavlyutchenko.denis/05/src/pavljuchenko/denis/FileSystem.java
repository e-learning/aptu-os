package pavljuchenko.denis;

import com.sun.org.apache.xpath.internal.operations.Bool;

import java.io.*;
import java.io.File;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.*;
import java.util.regex.Pattern;

public class FileSystem {
    private String rootPath;
    private Directory rootDirectory = null;
    private BitMap blocksMap = null;

    public FileSystem(String rootPath, int numberOfBlocks, int sizeOfBlock)
    {
        if (rootPath == null || rootPath.isEmpty())
        {
            throw new NullPointerException("rootPath");
        }

        Path path = Paths.get(rootPath);

        if (!Files.exists(path)) {
            throw new IllegalArgumentException("There is no a folder " + rootPath);
        }

        this.rootPath = rootPath;

        this.blocksMap = new BitMap(this.rootPath, numberOfBlocks, sizeOfBlock);

//        ArrayList<IFile> files = blocksMap.getAllIFiles();
//
//        int a = 5;
    }

    public void init() throws IOException {
        try {
            this.blocksMap.init();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    public void move(String src, String dst)
    {
        if (src == null || src.isEmpty()) {
            throw new NullPointerException("src");
        }

        if (dst == null || dst.isEmpty()) {
            throw new NullPointerException("dst");
        }

        copy(src, dst);
        rm(src);
    }

    public void rm(String path)
    {
        if (path == null || path.isEmpty())
        {
            throw new NullPointerException("path");
        }

        pavljuchenko.denis.IFile iFile = blocksMap.exportFile(path);

        if (iFile == null)
        {
            throw new NullPointerException("There is no a file " + path);
        }

        Directory parentDirectory = iFile.getParentDirectory();

        if (parentDirectory != null)
        {
            parentDirectory.removeFile(iFile);
        }

        if (iFile.isDirectory())
        {
            blocksMap.removeDirectory(iFile);
        }
        else
        {
            blocksMap.removeFile(iFile);
        }
    }

    public void copy(String src, String dst)
    {
        if (src == null || src.isEmpty())
        {
            throw new NullPointerException("src");
        }

        if (dst == null || dst.isEmpty())
        {
            throw new NullPointerException("dst");
        }

        pavljuchenko.denis.IFile iFile = blocksMap.exportFile(src);

        if (iFile.isDirectory())
        {
            copyDirectory(iFile, src, dst);
        }
        else
        {
            copyFile(iFile, dst);
        }
    }

    private void copyDirectory(pavljuchenko.denis.IFile directory, String src, String dst)
    {
        Directory dir = (Directory)directory;

        ArrayList<IFile> files = blocksMap.getAllIFiles();

        ArrayList<IFile> filesStartWith = new ArrayList<>();
        ArrayList<IFile> directoriesStartWith = new ArrayList<>();

        for (IFile f : files)
        {
            String fullPath = f.getFullPath();

            if (fullPath == null || fullPath.isEmpty())
            {
                continue;
            }

            if (fullPath.startsWith(dir.getFullPath())) {
                if (f.isDirectory())
                {
                    directoriesStartWith.add(f);
                }
                else
                {
                    filesStartWith.add(f);
                }
            }
        }

        for (IFile d : directoriesStartWith)
        {
            String newPath = d.getFullPath().replace(src,dst);

            try
            {
                mkdir(newPath);
            }
            catch (Exception ignored){}
        }

        for (IFile f : filesStartWith)
        {
            pavljuchenko.denis.File newFile = (pavljuchenko.denis.File)f;

            String newPath = f.getFullPath().replace(src,dst);

            try
            {
                createFile(newPath, newFile.getData());
            }
            catch (Exception ignored){}
        }
    }

    private void copyFile(pavljuchenko.denis.IFile file, String dst)
    {
        pavljuchenko.denis.File f = (pavljuchenko.denis.File)file;
        createFile(dst, f.getData());
    }

    public void ls(String fsFilePath)
    {
        if (fsFilePath == null || fsFilePath.isEmpty())
        {
            throw new NullPointerException("fsFilePath");
        }

        pavljuchenko.denis.IFile iFile = blocksMap.exportFile(fsFilePath);

        if (iFile.isDirectory())
        {
            HashMap<String, String> hashMap = new HashMap<>();
            ArrayList<IFile> files = blocksMap.getAllIFiles();

            for (IFile f : files)
            {
                if (f.getParentDirectory() == null)
                {
                    continue;
                }

                if (f.getParentDirectory().getFullPath().equals(iFile.getFullPath()))
                {
                    String t = f.isDirectory() ? "d" : "f";
                    hashMap.put(f.getName(), t);
                }
            }

            StringBuilder stringBuilder = new StringBuilder();

            for (Map.Entry<String, String> entry : hashMap.entrySet())
            {
                stringBuilder.append(entry.getKey());
                stringBuilder.append(" ");
                stringBuilder.append(entry.getValue());
                stringBuilder.append("\r\n");
            }

            System.out.println(stringBuilder.toString());
        }
        else
        {
            System.out.println(iFile);
        }
    }

    public void exportFile(String fsFilePath, String hostFilePath)
    {
        if (hostFilePath == null || hostFilePath.isEmpty())
        {
            throw new NullPointerException("hostFilePath");
        }

        if (fsFilePath == null || fsFilePath.isEmpty())
        {
            throw new NullPointerException("fsFilePath");
        }

        pavljuchenko.denis.File file = (pavljuchenko.denis.File)blocksMap.exportFile(fsFilePath);

        Path path = Paths.get(hostFilePath);

        byte[] data = Arrays.copyOf(file.getData(), file.getSizeOfFile());

        try {
            Files.write(path, data);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void importFile(String hostFilePath, String fsFilePath)
    {
        if (hostFilePath == null || hostFilePath.isEmpty())
        {
            throw new NullPointerException("hostFilePath");
        }

        Path path = Paths.get(hostFilePath);

        if (!Files.exists(path)) {
            throw new IllegalArgumentException("There is no a file " + hostFilePath);
        }

        try {
            createFile(fsFilePath, Files.readAllBytes(path));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void createFile(String fsFilePath, byte[] d)
    {
        if (fsFilePath == null || fsFilePath.isEmpty())
        {
            throw new NullPointerException("fsFilePath");
        }

        Directory parentDirectory = getParentDirectory(fsFilePath);

        if (parentDirectory == null)
        {
            throw new IllegalArgumentException("There is no a directories for " + fsFilePath);
        }

        String pattern = Pattern.quote(System.getProperty("file.separator"));
        String[] data = fsFilePath.split(pattern);

        String fileName = data[data.length - 1];

        try {
            pavljuchenko.denis.File file = new pavljuchenko.denis.File(fileName, parentDirectory, d);

            ArrayList<IFile> allFiles = blocksMap.getAllFiles();

            for (IFile iFile : allFiles)
            {
                if (file.getFullPath().equals(iFile.getFullPath()))
                {
                    throw new IllegalArgumentException("File " + file.getFullPath() + " already exists.");
                }
            }

            blocksMap.importFile(file);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void mkdir(String path)
    {
        if (path == null || path.isEmpty())
        {
            throw new NullPointerException("path");
        }

        ArrayList<IFile> directories = blocksMap.getAllDirectories();

        String pattern = Pattern.quote(System.getProperty("file.separator"));
        String[] data = path.split(pattern);

        ArrayList<String> needCreateDirectories = new ArrayList<>();

        for (int i = 0; i < data.length; ++i)
        {
            Boolean isNeedCreate = true;

            for (IFile directory : directories)
            {
                if (directory.getFullPath().equals(data[i]))
                {
                    isNeedCreate = false;
                    break;
                }
            }

            if (isNeedCreate)
            {
                needCreateDirectories.add(data[i]);
            }
        }

        String prevNameDirectory = "";

        for (int i = 0; i < data.length; ++i)
        {
            if (data[i].equals(needCreateDirectories.get(0)))
            {
                prevNameDirectory = data[i];
            }
            else
            {
                break;
            }
        }

        if (prevNameDirectory.isEmpty())
        {
            prevNameDirectory = File.separator;
        }

        IFile prevDirectory = null;

        for (IFile dir : directories)
        {
            if (dir.getName().equals(prevNameDirectory))
            {
                prevDirectory = dir;
                break;
            }
        }

        ArrayList<Directory> dirictoriesForCreation = new ArrayList<>();

        for (int i = 0; i < needCreateDirectories.size(); ++i)
        {
            if (needCreateDirectories.get(i).isEmpty())
            {
                continue;
            }

            Directory d = null;

            Boolean isHaveDir = false;

            for (IFile dir : directories)
            {
                String fullPath = File.separator;

                for (int j = 1; j <= i; ++j)
                {
                    fullPath += needCreateDirectories.get(j);

                    if (j < i)
                    {
                        fullPath += File.separator;
                    }
                }

                if (dir.getFullPath().equals(fullPath))
                {
                    d = (Directory) dir;
                    isHaveDir = true;
                    break;
                }
            }

            if (!isHaveDir)
            {
                Directory parent = (Directory) prevDirectory;

                d = new Directory(needCreateDirectories.get(i), parent);

                dirictoriesForCreation.add(d);
            }

            prevDirectory = d;
        }

        for (Directory dir : dirictoriesForCreation)
        {
            try {
                blocksMap.createDirectory(dir);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void format()
    {
        rootDirectory = Directory.CreateRootDirectory();

        ArrayList<IFile> directories = blocksMap.getAllDirectories();

        for (IFile directory : directories)
        {
            if (directory.getFullPath().equals(rootDirectory.getFullPath()))
            {
                throw new IllegalArgumentException("Root directory has already existed.");
            }
        }

        try {
            blocksMap.createDirectory(rootDirectory);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public String getRootPath() {
        return rootPath;
    }

    public Directory getRootDirectory() {
        return rootDirectory;
    }

    private Directory getParentDirectory(String fsFilePath)
    {
        ArrayList<IFile> directories = blocksMap.getAllDirectories();

        String pattern = Pattern.quote(System.getProperty("file.separator"));
        String[] data = fsFilePath.split(pattern);

        String fullDirectoryPath = File.separator;

        for (int i = 1; i < data.length - 1; ++i)
        {
            fullDirectoryPath += data[i];

            if (i < data.length - 2)
            {
                fullDirectoryPath += File.separator;
            }
        }

        for (IFile directory : directories)
        {
            if (directory.getFullPath().equals(fullDirectoryPath))
            {
                return (Directory) directory;
            }
        }

        return null;
    }
}
