#include "Const.h"
#include "command.h"
#include "converter.h"
#include "structures.h"
#include <fstream>

size_t init_fileSystem(std::string const & root)
{
    size_t blocksize = 0;
    size_t blockscount = 0;
    std::fstream configfs ((root + "/" + CONFIG).c_str(),
                           std::fstream::in | std::fstream::out | std::fstream::binary);
    if (configfs.is_open()) {
        configfs >> blocksize;
        configfs >> blockscount;
    }
    else {
        return ERROR_READ_CONFIG;
    }
    std::fstream blockfs;
    for (size_t i = 0; i < blockscount; ++i) {
        std::string filename = root + "/" + number_to_string(i);
        blockfs.open(filename.c_str(),
                     std::fstream::out | std::fstream::binary | std::fstream::trunc);
        //fill_block_by_char(blockfs, blocksize, '\0');
        blockfs.close();
    }

    return SUCCESS;
}

size_t format(std::string const & rootpath)
{
    size_t result = init_fileSystem(rootpath);
    if (result != SUCCESS) {
        return result;
    }
    size_t blocksize = 0;
    size_t blockscount = 0;
    std::fstream configfs ((rootpath + "/" + CONFIG).c_str(),
                           std::fstream::in | std::fstream::binary);
    if (configfs.is_open()) {
        configfs >> blocksize;
        configfs >> blockscount;
    }
    else {
        return ERROR_READ_CONFIG;
    }
    configfs.close();

    Dir rootDir ("/", 0, get_currenttime(), blockscount / (blocksize * BITS_PER_BYTE) + 2);
    Root root (rootpath, blockscount - 1, blocksize, blockscount / (blocksize * BITS_PER_BYTE) + 1, blockscount, rootDir);
    Bitmap bitmap(root.get_bitmapCount());
    bitmap.initWriteSelf(root);

    root.writeSelf();
    root.get_rootDir().writeSelf(root, bitmap);

    return SUCCESS;
}

size_t create_dirpath (Root & root, Bitmap & bitmap, Dir & parentDir, vecStrIt current, vecStrIt end)
{
    if (current == end) {
        return SUCCESS;
    }
    std::string localname = *current;
    if (parentDir.find_file_by_name(localname) != LAST_BLOCK) {
        return ERROR_INCORRECT_PATH;
    }
    size_t subdirNumber = parentDir.find_dir_by_name(*current);
    if (subdirNumber != LAST_BLOCK) {
        Dir & subdir = parentDir.get_subdir(subdirNumber);
        size_t result = create_dirpath(root, bitmap, subdir, current + 1, end);
        return result;
    }
    if (root.get_freeBlockscount() < (size_t)(end - current) + 1) {
        return ERROR_NOT_ENOUGH_SPACE;
    }
    size_t subdirStartBlockNumber = bitmap.get_nextfreeblock(root);
    bitmap.set_blockBusy(subdirStartBlockNumber, root);
    Dir subdir(*current, 0, get_currenttime(), subdirStartBlockNumber);
    parentDir.add_dirrecord(subdir);
    return create_dirpath(root,bitmap, parentDir.get_lastdir(), current + 1, end);
}

size_t import (Root & root, Bitmap & bitmap, Dir & parentDir, vecStrIt current, vecStrIt end, std::string hostfile)
{
    if (current == end) {
        if ((parentDir.find_file_by_name(*current) != LAST_BLOCK) || (parentDir.find_dir_by_name(*current) != LAST_BLOCK)) {
            return ERROR_INCORRECT_PATH;
        }
        size_t startblockNumber = bitmap.get_nextfreeblock(root);
        File file (*(current), get_filesize_by_name(hostfile), get_currenttime(), startblockNumber);
        size_t result = file.read_hostfile(hostfile);
        if (result != SUCCESS) {
            return result;
        }
        parentDir.add_filerecord(file);
        return SUCCESS;
    }
    size_t subdirNumberBlock = parentDir.find_dir_by_name(*current);
    if (subdirNumberBlock == LAST_BLOCK) {
        return ERROR_INCORRECT_PATH;
    }
    Dir & subdir = parentDir.get_subdir(subdirNumberBlock);
    return import(root, bitmap, subdir, current + 1, end, hostfile);
}

size_t exportFile (Root & root, Bitmap & bitmap, Dir & parentDir, vecStrIt current, vecStrIt end, std::string hostfile)
{
    if (current == end) {
        size_t filenumber = parentDir.find_file_by_name(*current);
        if (filenumber == LAST_BLOCK) {
            return ERROR_INCORRECT_PATH;
        }
        parentDir.get_file(filenumber).readSelf(root);
        parentDir.get_file(filenumber).write_hostfile(hostfile);
        return SUCCESS;
    }
    size_t subdirNumber = parentDir.find_dir_by_name(*current);
    if (subdirNumber == LAST_BLOCK) {
        return ERROR_INCORRECT_PATH;
    }
    Dir & subdir = parentDir.get_subdir(subdirNumber);
    return exportFile(root, bitmap, subdir, current + 1, end, hostfile);
}

size_t print_ls (Root & root, Bitmap & bitmap, Dir & parentDir, vecStrIt current, vecStrIt end)
{
    if (current == end) {
        parentDir.printInfo();
        return SUCCESS;
    }
    size_t subdirBlockNumber = parentDir.find_dir_by_name(*current);
    if (subdirBlockNumber != LAST_BLOCK) {
        Dir & subdir = parentDir.get_subdir(subdirBlockNumber);
        return print_ls(root, bitmap, subdir, current + 1, end);
    }
    size_t fileBlockNumber = parentDir.find_file_by_name(*current);
    if ((fileBlockNumber != LAST_BLOCK) && (current == (end - 1))) {
        File & file = parentDir.get_file(fileBlockNumber);
        file.printInfo();
        return SUCCESS;
    }
    return ERROR_INCORRECT_PATH;
}

size_t delete_rm (Root & root, Bitmap & bitmap, Dir & parentDir, vecStrIt current, vecStrIt end)
{
    if (current == end) {
        size_t recordNumber = parentDir.find_dir_by_name(*current);
        if (recordNumber == LAST_BLOCK) {
            recordNumber = parentDir.find_file_by_name(*current);
            if (recordNumber == LAST_BLOCK) {
                return ERROR_INCORRECT_PATH;
            }
            parentDir.remove_file(root, bitmap, recordNumber);
            return SUCCESS;
        }
        parentDir.remove_subdir(root, bitmap, recordNumber);
        return SUCCESS;
    }
    size_t subdirNumber = parentDir.find_dir_by_name(*current);
    if (subdirNumber != LAST_BLOCK) {
        Dir & subdir = parentDir.get_subdir(subdirNumber);
        return delete_rm(root, bitmap, subdir, current + 1, end);
    }
    return ERROR_INCORRECT_PATH;
}

//==brainFuck...but i hope it will be work...==//
size_t move (Root & root, Dir & rootDir, Bitmap & bitmap,
             Dir & parentSrcDir, vecStrIt current, vecStrIt end,
             std::vector<std::string> vpathDst, bool with_delete)
{
    if (current == end) {
        size_t dirname = parentSrcDir.find_dir_by_name(*current);
        size_t filename = parentSrcDir.find_file_by_name(*current);
        if (dirname != LAST_BLOCK) {
            Dir & movableDir = parentSrcDir.get_subdir(dirname);
            size_t requiredSpace = with_delete ? 1 : movableDir.get_sizeInBlocks() + 1;
            if (root.get_freeBlockscount() < requiredSpace) {
                return ERROR_NOT_ENOUGH_SPACE;
            }
            return try_to_move_dir(root, bitmap,
                                   rootDir, vpathDst.begin(), vpathDst.end(),
                                   parentSrcDir, dirname, with_delete);
        }
        if (filename != LAST_BLOCK) {
            File & movableFile = parentSrcDir.get_file(filename);
            size_t requiredSpace = with_delete ? 1 : movableFile.get_sizeInBlocks() + 1;
            if (root.get_freeBlockscount() < requiredSpace) {
                return ERROR_NOT_ENOUGH_SPACE;
            }
            return try_to_move_file(root, bitmap,
                                   rootDir, vpathDst.begin(), vpathDst.end(),
                                   parentSrcDir, filename, with_delete);
        }
        return ERROR_INCORRECT_PATH;
    }
    size_t dirNumber = parentSrcDir.find_dir_by_name(*current);
    if (dirNumber != LAST_BLOCK) {
        return move (root, rootDir, bitmap,
                     parentSrcDir.get_subdir(dirNumber), current + 1, end,
                     vpathDst, with_delete);
    }
    return ERROR_INCORRECT_PATH;
}

size_t try_to_move_dir (Root & root, Bitmap & bitmap,
                        Dir & parentDstDir, vecStrIt current, vecStrIt end,
                        Dir & parentOfMovable, size_t movableDirNumber,
                        bool with_delete)
{
    if (current == end) { //in folder
        parentDstDir.copy_dirrecord(root, bitmap, parentOfMovable.get_subdir(movableDirNumber));
        if (with_delete) {
            parentOfMovable.remove_subdir(root, bitmap, movableDirNumber);
        }
        return SUCCESS;
    }
    if (current == end - 1) {
        if (parentDstDir.find_file_by_name(*current) != LAST_BLOCK) {
            return ERROR_INCORRECT_PATH;
        }
        if (parentDstDir.find_dir_by_name(*current) == LAST_BLOCK) {
            parentDstDir.add_dirrecord_by_name (*current, root, bitmap);
        }
    }
    size_t dirNumber = parentDstDir.find_dir_by_name(*current);
    if (dirNumber != LAST_BLOCK) {
        return try_to_move_dir(root, bitmap,
                               parentDstDir.get_subdir(dirNumber), current + 1, end,
                               parentOfMovable, movableDirNumber, with_delete);
    }
    return ERROR_INCORRECT_PATH;
}

size_t try_to_move_file (Root & root, Bitmap & bitmap,
                         Dir & parentDstDir, vecStrIt current, vecStrIt end,
                         Dir & parentOfMovable, size_t movableFileNumber,
                         bool with_delete)
{
    if (current == end) { //in folder
        parentDstDir.copy_filerecord(root, bitmap, parentOfMovable.get_file(movableFileNumber));
        if (with_delete) {
            parentOfMovable.remove_file(root, bitmap, movableFileNumber);
        }
        return SUCCESS;
    }
    if ((current == end - 1) && (parentDstDir.find_dir_by_name(*current) == LAST_BLOCK)) {
        size_t fileNumber = parentDstDir.find_file_by_name(*current);
        if (fileNumber == LAST_BLOCK) {
            fileNumber = parentDstDir.add_filerecord_by_name(*current, root, bitmap);
        }
        parentDstDir.get_file(fileNumber).read_fsfile(root, parentOfMovable.get_file(movableFileNumber));
        if (with_delete) {
            parentOfMovable.remove_file(root, bitmap, movableFileNumber);
        }
        return SUCCESS;
    }
    size_t dirNumber = parentDstDir.find_dir_by_name(*current);
    if (dirNumber != LAST_BLOCK) {
        return try_to_move_file(root, bitmap,
                                parentDstDir.get_subdir(dirNumber), current + 1, end,
                                parentOfMovable, movableFileNumber,
                                with_delete);
    }
    return ERROR_INCORRECT_PATH;
}








