package pavljuchenko.denis;

import java.util.ArrayList;

public interface IFile {
    String getName();
    Boolean isDirectory();
    String getFullPath();
    ArrayList<Block> getBlocks();
    Directory getParentDirectory();
}
