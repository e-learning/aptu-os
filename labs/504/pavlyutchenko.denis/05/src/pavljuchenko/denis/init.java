package pavljuchenko.denis;

import java.io.File;
import java.io.IOException;
import java.util.Scanner;

public class init {

    public static void main(String[] args) throws IOException {

        final String rootPath = args[0];
        final String configPath = rootPath + File.separator + "config";

        int numberOfBlocks = 0;
        int sizeOfBlock = 0;

        Scanner scanner = new Scanner(new File(configPath));
        sizeOfBlock = scanner.nextInt();
        numberOfBlocks = scanner.nextInt();

        FileSystem fileSystem = new FileSystem(rootPath, numberOfBlocks, sizeOfBlock);
        fileSystem.init();
    }
}
