package pavljuchenko.denis;

import java.io.*;
import java.io.File;

public class Main {

    public static void main(String[] args) throws IOException {
        String rootPath = "C:\\Users\\Denis\\Desktop\\fs";
//        String hostFilePath = "C:\\Users\\Denis\\Desktop\\node-restfull\\README.md";
        String hostFilePath = "C:\\Users\\Denis\\Desktop\\node-restfull\\express.test.js";
//        String hostFilePathExport = "C:\\Users\\Denis\\Desktop\\node-restfull\\express.test-export.js";
        String hostFilePathExport = "C:\\Users\\Denis\\Desktop\\node-restfull\\README.md-export";
//        String fsFilePath = java.io.File.separator + "directory1" + File.separator + "README.md";
        String fsFilePath = java.io.File.separator + "directory1" + File.separator + "express.test.js";
        String copyFsFilePath = java.io.File.separator + "directory1" + File.separator + "directory2" + File.separator + "express.test.js";
        String moveFsFilePath = java.io.File.separator + "directory1" + File.separator + "directory2" + File.separator + "express.test-move.js";

        FileSystem fileSystem = new FileSystem(rootPath, 1000, 1024);

//        fileSystem.move(java.io.File.separator + "directory1", java.io.File.separator + "directory4");

//        fileSystem.rm(java.io.File.separator + "directory1");

//        fileSystem.ls(java.io.File.separator + "directory1");

//        fileSystem.exportFile(fsFilePath, hostFilePathExport);

//        fileSystem.init();
//
//        fileSystem.format();
//
//        fileSystem.mkdir(java.io.File.separator + "directory1" + java.io.File.separator + "directory2" + java.io.File.separator + "directory3");
//
//        fileSystem.importFile(hostFilePath, fsFilePath);
//
//        fileSystem.copy(fsFilePath, copyFsFilePath);
//        fileSystem.copy(java.io.File.separator + "directory1", java.io.File.separator + "directory4");
    }
}
