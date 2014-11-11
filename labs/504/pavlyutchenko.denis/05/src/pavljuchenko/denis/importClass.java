package pavljuchenko.denis;

import java.io.IOException;
import java.util.Scanner;

public class importClass {

    public static void main(String[] args) throws IOException {

        final String rootPath = args[0];
        final String configPath = rootPath + java.io.File.separator + "config";

        final String hostFilePath = args[1];
        final String fsFilePath = args[2];

        int numberOfBlocks = 0;
        int sizeOfBlock = 0;

        Scanner scanner = new Scanner(new java.io.File(configPath));
        sizeOfBlock = scanner.nextInt();
        numberOfBlocks = scanner.nextInt();

        FileSystem fileSystem = new FileSystem(rootPath, numberOfBlocks, sizeOfBlock);
        fileSystem.importFile(hostFilePath, fsFilePath);
    }
}
