package pavljuchenko.denis;

import java.io.*;
import java.io.File;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.*;

public class BitMap {
    private Map<Block, IFile> fileMap = new HashMap<>();
    private String rootPath;
    private int numberOfBlocks;
    private int sizeOfBlock;

    public BitMap(String rootPath, int numberOfBlocks, int sizeOfBlock)
    {
        if (numberOfBlocks < 2)
        {
            throw new IllegalArgumentException("numberOfBlocks < 2");
        }

        if (sizeOfBlock < 1024)
        {
            throw new IllegalArgumentException("sizeOfBlock < 1024");
        }

        this.numberOfBlocks = numberOfBlocks;
        this.sizeOfBlock = sizeOfBlock;

        if (rootPath == null || rootPath.isEmpty())
        {
            throw new NullPointerException("rootPath");
        }

        Path path = Paths.get(rootPath);

        if (!Files.exists(path)) {
            throw new IllegalArgumentException("There is no a folder " + rootPath);
        }

        this.rootPath = rootPath;

        try {
            if (needDeserialization())
            {
                fileMap = deserialize();
            }
        } catch (ClassNotFoundException | IOException e) {
            e.printStackTrace();
        }
    }

    public void init() throws IOException, ClassNotFoundException {
        for (int i = 0; i < numberOfBlocks; ++i)
        {
            Block b = new Block(i, sizeOfBlock);
            fileMap.put(b, null);

            String path = rootPath + java.io.File.separator + "block" + b.getId();
            java.io.File f = new java.io.File(path);

            byte dataToWrite[] = {};
            FileOutputStream out = new FileOutputStream(f);
            out.write(dataToWrite);
            out.close();
        }

        serialize(fileMap);
    }

    void importFile(pavljuchenko.denis.File file) throws IOException {
        if (file == null)
        {
            throw new NullPointerException("file");
        }

        byte[][] chunks = divideArray(file.getData(), sizeOfBlock);


        HashMap<Block, IFile> availableBlock = new HashMap<>();

        for (Map.Entry<Block, IFile> entry : fileMap.entrySet())
        {
            if (entry.getValue() == null)
            {
                availableBlock.put(entry.getKey(), entry.getValue());
            }
        }

        if (availableBlock.size() < chunks.length)
        {
            throw new OutOfMemoryError("Not enough memory to save file");
        }

        int index = 0;

        for (Map.Entry<Block, IFile> entry : availableBlock.entrySet())
        {
            fileMap.put(entry.getKey(), file);
            file.getBlocks().add(entry.getKey());
            ++index;

            if (index == chunks.length)
            {
                break;
            }
        }

        serialize(fileMap);
//
//        int counter = 0;
//
//        for (Block block : file.getBlocks())
//        {
//            String blockPath = rootPath + File.separator + "block" + block.getId();
//            Path pathBlock = Paths.get(blockPath);
//
//            Files.write(pathBlock, chunks[counter]);
//            ++counter;
//        }
    }

    public boolean needDeserialization() throws FileNotFoundException {
        String firstBlockPathName = rootPath + File.separator + "block0";
        Path path = Paths.get(firstBlockPathName);

        if (!Files.exists(path)) {
            return false;
        }

        Scanner scanner = new Scanner(new java.io.File(firstBlockPathName));

        int size = scanner.nextInt();

        return size > 0;
    }

    public ArrayList<IFile> getAllIFiles()
    {
        if (fileMap == null)
        {
            throw new NullPointerException("fileMap");
        }

        ArrayList<IFile> files = new ArrayList<>();

        for (Map.Entry<Block, IFile> entry : fileMap.entrySet())
        {
            if (entry.getValue() != null)
            {
                files.add(entry.getValue());
            }
        }

        return files;
    }

    public ArrayList<IFile> getAllFiles()
    {
        ArrayList<IFile> files = getAllIFiles();
        ArrayList<IFile> result = new ArrayList<>();

        for (IFile file : files)
        {
            if (!file.isDirectory())
            {
                result.add(file);
            }
        }

        return result;
    }

    public ArrayList<IFile> getAllDirectories()
    {
        ArrayList<IFile> files = getAllIFiles();
        ArrayList<IFile> result = new ArrayList<>();

        for (IFile file : files)
        {
            if (file.isDirectory())
            {
                result.add(file);
            }
        }

        return result;
    }

    public void createDirectory(Directory directory) throws IOException {
        if (directory == null)
        {
            throw new NullPointerException("directory");
        }

        if (fileMap == null)
        {
            throw new NullPointerException("fileMap");
        }

        Block key = null;

        for (Map.Entry<Block, IFile> entry : fileMap.entrySet())
        {
            if (entry.getValue() == null)
            {
                key = entry.getKey();
                break;
            }
        }

        if (key == null)
        {
            throw new OutOfMemoryError("Not enough memory to save directory");
        }

        fileMap.remove(key);

        directory.getBlocks().add(key);
        fileMap.put(key, directory);

        serialize(fileMap);
    }

    public void removeDirectory(IFile file)
    {
        if (file == null)
        {
            throw new NullPointerException("file");
        }

        for (Map.Entry<Block, IFile> entry : fileMap.entrySet())
        {
            if (entry.getValue() == null)
            {
                continue;
            }

            String fullPath = entry.getValue().getFullPath();
            String fullPathRemovedFile = file.getFullPath();

            if (fullPath.equals(fullPathRemovedFile) || fullPath.startsWith(fullPathRemovedFile))
            {
                fileMap.put(entry.getKey(), null);
            }
        }

        try {
            serialize(fileMap);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void removeFile(IFile file)
    {
        if (file == null)
        {
            throw new NullPointerException("file");
        }

        ArrayList<Block> blocks = file.getBlocks();

        for (Block b : blocks)
        {
            fileMap.put(b, null);
        }

        try {
            serialize(fileMap);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public IFile exportFile(String fsFilePath) {
        if (fsFilePath == null || fsFilePath.isEmpty())
        {
            throw new NullPointerException("fsFilePath");
        }

        IFile file = null;

        ArrayList<IFile> files = getAllIFiles();

        for (IFile f : files)
        {
            if (f.getFullPath().equals(fsFilePath))
            {
                file = f;
                break;
            }
        }

        if (file == null)
        {
            throw new IllegalArgumentException("There is no a file " + fsFilePath);
        }

        return file;
    }

    private void serialize(Map<Block, IFile> d) throws IOException {
        byte[] data = Serializer.serialize(fileMap);

        byte[][] chunks = divideArray(data, sizeOfBlock);

        for (int i = 1; i <= chunks.length; ++i)
        {
            String blockPath = rootPath + File.separator + "block" + i;
            Path pathBlock = Paths.get(blockPath);

            Files.write(pathBlock, chunks[i - 1]);
        }

        String firstBlockPathName = rootPath + File.separator + "block0";
        Writer wr = new FileWriter(firstBlockPathName);
        wr.write(String.valueOf(chunks.length));
        wr.close();
    }

    private HashMap<Block, IFile> deserialize() throws IOException, ClassNotFoundException {
        String firstBlockPathName = rootPath + File.separator + "block0";
        Scanner scanner = new Scanner(new java.io.File(firstBlockPathName));

        HashMap<Block, IFile> result = null;

        int size = scanner.nextInt();

        byte[][] byte_arrays = new byte[size][sizeOfBlock];

        int lenAllArrays = 0;

        for (int i = 0; i < size; ++i)
        {
            String p = rootPath + java.io.File.separator + "block" + (i + 1);
            Path pathBlock = Paths.get(p);

            int len = Files.readAllBytes(pathBlock).length;
            lenAllArrays += len;

            byte_arrays[i] = Arrays.copyOf(Files.readAllBytes(pathBlock), len);
        }

        byte[] b = concatAll(byte_arrays);

        result = (HashMap<Block, IFile>) Serializer.deserialize(concatAll(byte_arrays));

        return result;
    }

    private static byte[] concatAll(byte[]... rest) {
        int totalLength = 0;
        for (byte[] array : rest) {
            totalLength += array.length;
        }

        byte[] b = new byte[0];

        byte[] result = Arrays.copyOf(b, totalLength);
        int offset = b.length;
        for (byte[] array : rest) {
            System.arraycopy(array, 0, result, offset, array.length);
            offset += array.length;
        }
        return result;
    }

    private static byte[][] divideArray(byte[] source, int chunksize) {

        byte[][] ret = new byte[(int)Math.ceil(source.length / (double)chunksize)][chunksize];

        int start = 0;

        for (byte[] aRet : ret) {
            if (start + chunksize > source.length) {
                System.arraycopy(source, start, aRet, 0, source.length - start);
            } else {
                System.arraycopy(source, start, aRet, 0, chunksize);
            }
            start += chunksize;
        }

        return ret;
    }
}
