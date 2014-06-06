#include "../main/filesystem.h"

int main(int argc, char *argv[])
{
    if(argc != 4) {
        std::cout << "wrong arg number!  Usage: export <root path> <file in fs> <target>" << std::endl;
        return 1;
    }

    File_system fs(argv[1]);
    
    fs.load();

    if(fs.export_file(argv[2], argv[3]) == 0) {
        std::cout << "export failed on"<< argv[3] << std::endl;
        return 1;
    }
    
    fs.save();

    return 0;
}
