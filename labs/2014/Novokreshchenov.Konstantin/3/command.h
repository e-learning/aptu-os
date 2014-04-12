#ifndef COMMAND_H
#define COMMAND_H

#include "structures.h"
#include <string>
#include <vector>

typedef std::vector<std::string>::iterator vecStrIt;

size_t init_fileSystem (std::string const & root);
size_t format (std::string const & root);
size_t create_dirpath (Root & root, Bitmap & bitmap, Dir & parentDir, vecStrIt current, vecStrIt end);
size_t import (Root & root, Bitmap & bitmap, Dir & parentDir, vecStrIt current, vecStrIt end, std::string hostfile);
size_t exportFile (Root & root, Bitmap & bitmap, Dir & parentDir, vecStrIt current, vecStrIt end, std::string hostfile);
size_t print_ls (Root & root, Bitmap & bitmap, Dir & parentDir, vecStrIt current, vecStrIt end);
size_t delete_rm (Root & root, Bitmap & bitmap, Dir & parentDir, vecStrIt current, vecStrIt end);

size_t move (Root & root, Dir & rootDir, Bitmap & bitmap,
             Dir & parentSrcDir, vecStrIt current, vecStrIt end,
             std::vector<std::string> vpathDst, bool with_delete);
size_t try_to_move_dir (Root & root, Bitmap & bitmap,
                        Dir & parentDstDir, vecStrIt current, vecStrIt end,
                        Dir & parentOfMovable, size_t movableDirNumber,
                        bool with_delete);
size_t try_to_move_file (Root & root, Bitmap & bitmap,
                         Dir & parentDstDir, vecStrIt current, vecStrIt end,
                         Dir & parentOfMovable, size_t movableFileNumber,
                         bool with_delete);

#endif // COMMAND_H
