
#include "filesystem.h"
#include "directory_iterator.h"
#include <cstring>
#include <string>
using std::pair;
using std::string;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::cout;


File_system::File_system(const char*  rootPath) : ready(false), Size_of_superblock(0), N(0), path(rootPath), blocks(0) {
    if(path[path.size()-1] != '/') path.append("/"); //formalize path
    string config = config_path(rootPath);
    if(!config.empty()) {//init config file
        ifstream configFile(config.c_str());
        if(!configFile.is_open()) {
            return;
        }
        configFile >> Size_of_superblock >> N;
        configFile.close();
    } else {//init without config
        string super_block_path(rootPath);
        super_block_path.append(super_block_path[super_block_path.size()-1] == '/' ? "0" : "/0");
        FILE*  super_block_file = fopen(super_block_path.c_str(), "rb");
        if(!super_block_file) {
            return;
        }
        Super_block_info tmpsuper_block;
        fread(&tmpsuper_block, sizeof(Super_block_info), 1, super_block_file);
        Size_of_superblock = tmpsuper_block.block_size;
        N = tmpsuper_block.number_of_blocks;
        fclose(super_block_file);
    }
    flag = 1;
    blocks = new Block_pack(Size_of_superblock, path);

    ready = true;
}

File_system::~File_system() {
    delete blocks;
}


//create blocks filled with 0
bool File_system::init() {
    if(!ready) return false;
    char buf[10];
    for(int i=0; i<N; i++) {
        string blockName = path;
        sprintf(buf, "%d", i);
        blockName.append(buf);
        ofstream bf(blockName.c_str(), ios_base::out | ios_base::binary);
        if(!bf.is_open()) {
            return false;
        }
        bf.close();
    }
    return true;
}

//creating root
bool File_system::format() {
    if(!ready) return false;
//init super block info
    super_block.number_of_blocks = N;
    super_block.block_size = Size_of_superblock;
    super_block.bitmap_size = N/8+1;
    super_block.bitmap = new char[super_block.bitmap_size];
    memset(super_block.bitmap, 0, sizeof(char)*super_block.bitmap_size);
    int curBlockSize = Size_of_superblock - sizeof(Super_block_info);
    super_block_length = super_block.bitmap_size > curBlockSize ? (super_block.bitmap_size-curBlockSize)/(Size_of_superblock-sizeof(Block_header))+2 : 1;
    super_block.next_block = super_block_length > 1 ? 1 : -1;
    super_block.root_block = super_block_length;
    for(int i=0; i<=super_block_length; i++) reserveBlock(i);

    writeSuperBlock();

    FILE*  blockFile = fopen(getBlockPath(super_block_length).c_str(), "wb");
    Block_header bh;
    bh.next_block = -1;
    fwrite(&bh, sizeof(Block_header), 1, blockFile);
//create root
    Directory_header dh;
    dh.files_in_dir = 0;
    fwrite(&dh, sizeof(Directory_header), 1, blockFile);

    root.attr = FLAG_DIR | EXIST_FLAG;
    root.blocks = 1;
    root.filename[0] = '/';
    root.first_block = super_block_length;
    root.last_modified = time(0);
    root.size = sizeof(File_entry);
    fwrite(&root, sizeof(File_entry), 1, blockFile);
    fclose(blockFile);

    return true;
}

//load super_block
bool File_system::load() {
    if(!ready) return false;

    FILE*  file_block = fopen(getBlockPath(0).c_str(), "rb");
    fread(&super_block, sizeof(Super_block_info), 1, file_block);
    super_block.bitmap = new char[super_block.bitmap_size];
    super_block_length = 1;
    //load super_block
    if(super_block.next_block == -1) {
        fread(super_block.bitmap, sizeof(char), super_block.bitmap_size, file_block);
        fclose(file_block);
    } else {
        fread(super_block.bitmap, sizeof(char), Size_of_superblock-sizeof(Super_block_info), file_block);
        fclose(file_block);
        Block_header bh;
        bh.next_block = super_block.next_block;
        int mapWritten = Size_of_superblock-sizeof(Super_block_info);
        while (bh.next_block != -1) {
            file_block = fopen(getBlockPath(bh.next_block).c_str(), "rb");
            fread(&bh, sizeof(Block_header), 1, file_block);
            if(bh.next_block != -1) fread(super_block.bitmap+mapWritten, sizeof(char), Size_of_superblock-sizeof(Block_header), file_block);
            else fread(super_block.bitmap+mapWritten, sizeof(char), super_block.bitmap_size-mapWritten, file_block);
            fclose(file_block);
            mapWritten += Size_of_superblock-sizeof(Block_header);
            super_block_length++;
        }
    }

    Block_header bh;
    Directory_header dh;
    file_block = fopen(getBlockPath(super_block.root_block).c_str(), "rb");
    fread(&bh, sizeof(Block_header), 1, file_block);
    fread(&dh, sizeof(Directory_header), 1, file_block);
    fread(&root, sizeof(File_entry), 1, file_block);
    fclose(file_block);

    blocks->getBlock(super_block.root_block);

    return true;
}

//dump super_block
bool File_system::save() {
    if(!ready) return false;

    writeSuperBlock();
    blocks->save();

    return true;
}

// ---
/**
 * @brief File_system::create_file
 * @param path
 * @param alloc
 * @return pointer to File_entry or null if failed
 */
File_entry*  File_system::create_file(const string& path, bool alloc) {
    size_t lastSlash = path.find_last_of('/');
    if(lastSlash == string::npos || lastSlash == path.size()-1 ) {
        return 0;// dir??
    }

    string fname = path.substr(lastSlash+1);
    if(fname.size() > NAME_BOUND) {
        return 0;
    }

    File_entry*  oldf = findFile(path, true);
    if(oldf != 0) {
        return oldf; //oh! there is already one! =)
    }

    File_entry*  dir = findFile(path.substr(0, lastSlash));
    if(!dir) {
        return 0;//no dir
    }
    if(!(dir->attr&  FLAG_DIR)) {
        return 0;//dir is not dir
    }

    Directory_iterator directory_iterator(this, get_block(dir->first_block), false);
    File_entry*  _file_entry = directory_iterator.current();
    while(directory_iterator.hasNext() && _file_entry->attr&  EXIST_FLAG) {
        _file_entry = directory_iterator.next();
    }
    if(directory_iterator.atBorder()) {//need new block
        int inewblock = allocateBlock();
        if(inewblock == -1) {
            return 0;
        }
        directory_iterator.currentBlock()->get_header()->next_block = inewblock;
        Block*  newblock = get_block(inewblock);
        newblock->get_header()->next_block = -1;
        _file_entry = (File_entry*)newblock->get_data();
    } else {
        ++_file_entry;//new file in dir
    }

    _file_entry->attr = EXIST_FLAG;
    _file_entry->blocks = alloc ? 1 : 0;
    _file_entry->size = 0;
    strncpy(_file_entry->filename, fname.c_str(), NAME_BOUND);
    _file_entry->last_modified = time(0);
    _file_entry->first_block = alloc ? allocateBlock() : -1;

    if(alloc && _file_entry->first_block == -1 && flag) {
        return 0;
    }

    directory_iterator.dirHeader()->files_in_dir++;
    return _file_entry;
}

File_entry*  File_system::forced_create_file(const string& path, bool alloc) {
    size_t lastSlash = path.find_last_of('/');
    if(path[0] != '/' || lastSlash == string::npos || lastSlash == path.size()-1 ) {
        return 0;
    }

    File_entry*  oldf = findFile(path, true);
    if(oldf != 0) return oldf; //oh! there is already one! =)

    size_t current_position = 1;
    File_entry*  curDir =& root;
    while(1) {
        size_t next_slash = path.find_first_of('/', current_position);
        string ndir = next_slash != string::npos ? path.substr(current_position, next_slash-current_position) : path.substr(current_position);
        if(next_slash == string::npos) break; //toooo loooong

        if(ndir.size() > NAME_BOUND) {
            return 0;
        }

        curDir = findFile(path.substr(0, next_slash), true);
        if(!curDir) curDir = create_dir(path.substr(0, next_slash)); //create parent dir
        current_position = next_slash+1;
    }

    return create_file(path, alloc);
}

File_entry*  File_system::create_dir(const string& path) {
    File_entry*  dir = create_file(path, true);
    if(!dir) return 0;

    dir->attr |= FLAG_DIR;
    Directory_header*  dh = (Directory_header*)get_block(dir->first_block)->get_data();
    dh->files_in_dir = 0;
    File_entry*  ndir = (File_entry*)(get_block(dir->first_block)->get_data()+sizeof(Directory_header));
    ndir->copy(dir);
    return dir;
}

File_entry*  File_system::forced_create_dir(const string& path) {
    size_t lastSlash = path.find_last_of('/');
    if(path[0] != '/' || lastSlash == string::npos || lastSlash == path.size()-1 ) {
        return 0; //bad path
    }

    File_entry*  oldf = findFile(path, true);
    if(oldf != 0) return oldf; //oh! there is already one! =)

    size_t current_position = 1;
    File_entry*  curDir =& root;
    while(1) {
        size_t next_slash = path.find_first_of('/', current_position);
        string dir_name = next_slash != string::npos ? path.substr(current_position, next_slash-current_position) : path.substr(current_position);
        if(next_slash == string::npos) break; //toooooooo looooong

        if(dir_name.size() > NAME_BOUND) {
            return 0;
        }

        curDir = findFile(path.substr(0, next_slash), true);
        if(!curDir) curDir = create_dir(path.substr(0, next_slash));//creating parent dir
        current_position = next_slash+1;
    }

    return create_dir(path);
}

// ---

bool File_system::import_file(const string& src, const string& target) {
    FILE*  srcFile = fopen(src.c_str(), "rb");
    if(!srcFile) {
        return false;
    }

    fseek(srcFile, 0, SEEK_END);
    size_t file_size = ftell(srcFile); //get file size

    if(file_size > freeSpace()) {
        return false; //not enough space
    }

    fseek(srcFile, 0, SEEK_SET);//return to the begin of file
    char*  buf = new char[Size_of_superblock];


    File_entry*  destFile = findFile(target, true);
    if(destFile) {
        remove_file(target);//remove old
    }
    destFile = forced_create_file(target, true); //create new
    if(!destFile) return false; // failed on create new

    Block*  curBlock = get_block(destFile->first_block);
    size_t readed_counter = 0;
    size_t blockCount = 1;
    while(true) {
        size_t readed = fread(buf, sizeof(char), Size_of_superblock-sizeof(Block_header), srcFile);
        memcpy(curBlock->get_data(), buf, curBlock->get_size()); //read to buf_file_entryr
        readed_counter += readed;
        if(readed_counter >= file_size) {
            curBlock->get_header()->next_block = -1;//end_file
            break;
        }
        int newblock = allocateBlock();//create new block
        curBlock->get_header()->next_block = newblock;
        curBlock = get_block(newblock);
        blockCount++;
    }
    fclose(srcFile);

    //init file attributes
    destFile->blocks = blockCount;
    destFile->size = file_size;
    destFile->last_modified = time(0);
    return true;
}

bool File_system::export_file(const string& path, const string& target) {
    File_entry*  srcFile = findFile(path);
    if(!srcFile || srcFile->attr &  FLAG_DIR) return false; // no file or dir

    FILE*  targetFile = fopen(target.c_str(), "wb");
    size_t left = srcFile->size;
    Block*  curBlock = get_block(srcFile->first_block);
    for(int i=0; i<srcFile->blocks; i++) {
        if(left > curBlock->get_size()) {
            fwrite(curBlock->get_data(), sizeof(char), curBlock->get_size(), targetFile);
            left -= curBlock->get_size();
            curBlock = get_block(curBlock->get_header()->next_block);
        } else {
            fwrite(curBlock->get_data(), sizeof(char), left, targetFile);
            break;
        }
    }
    fclose(targetFile);
    return true;
}

// ---

bool File_system::remove_file(const string& path, bool recursive) {
    File_entry*  file_entry = findFile(path);
    if(!file_entry || (file_entry->attr &  FLAG_DIR)) return false; // no file or dir

    size_t last_slash = path.find_last_of('/');
    File_entry*  parent_dir = findFile(path.substr(0, last_slash));
    Directory_header*  parent_dir_header =
            (Directory_header*)get_block(parent_dir->first_block)->get_data();

    int current_block = file_entry->first_block;
    for(int i=0; i<file_entry->blocks; i++) {
        int next_block = get_block(current_block)->get_header()->next_block;
        free_block(current_block);
        if(next_block == -1) break; //reached last block
        current_block = next_block; //move to next
    }

    file_entry->attr ^= EXIST_FLAG;
    if(!recursive) parent_dir_header->files_in_dir--;

    return true;
}

bool File_system::remove(const string& path, bool recursive) {
    if(path == "/") return false; //use format instead
    File_entry*  file_entry = findFile(path);
    if(!file_entry) return false; //no file
    if(!(file_entry->attr & FLAG_DIR)) return remove_file(path, recursive);//exist and dir

    size_t last_slash = path.find_last_of('/');
    File_entry*  parent_dir = findFile(path.substr(0, last_slash));
    Directory_header*  parent_directory_header =
            (Directory_header*)get_block(parent_dir->first_block)->get_data();

    Directory_iterator directory_iterator(this, get_block(file_entry->first_block));
    File_entry*  current_dir_file_entry = directory_iterator.current();
    while(directory_iterator.hasNext()) {
        current_dir_file_entry = directory_iterator.next();
        stringstream s;
        s << path << "/" << current_dir_file_entry->name();
        remove(s.str(), true);
    }

    int current_block_number = file_entry->first_block;
    while(true) {
        if(current_block_number == -1) break; //reach the last block
        int next = get_block(current_block_number)->get_header()->next_block;
        free_block(current_block_number);
        current_block_number = next;
    }

    file_entry->attr ^= EXIST_FLAG;
    if(!recursive) parent_directory_header->files_in_dir--;

    return true;
}

// real madness =)
bool File_system::move(const string& src_path, const string& target_path) {

    if( target_path.size() >= src_path.size() && target_path.substr(0, src_path.size()) == src_path){
        return false; //cant move into itself
    }
    if(src_path == target_path) {
        return false; // no need to move
    }
    File_entry*  source_file_entry = findFile(src_path);
    if(!source_file_entry) {
        return false; //no source??
    }

    size_t last_slash = src_path.find_last_of('/');

    File_entry*  parent_dir = findFile(src_path.substr(0, last_slash));
    Directory_header*  parent_dir_header =
            (Directory_header*)get_block(parent_dir->first_block)->get_data();

    File_entry*  target_file_entry = findFile(target_path);

    if( target_file_entry != 0 && !(target_file_entry->attr &  FLAG_DIR)
            && (source_file_entry->attr &  FLAG_DIR)) {
        return false; // target is file, src is dir
    }

    //both files and both there
    if( target_file_entry != 0
            && !(target_file_entry->attr &  FLAG_DIR)
            && !(source_file_entry->attr &  FLAG_DIR)
            ) {

        string target_file_name = target_file_entry->name();
        string source_file_name = source_file_entry->name();
        string target_parent = "/";
        string source_parent = "/";

        size_t lastSlash = target_path.find_last_of('/');
        if(lastSlash != 0) {
            target_parent = target_path.substr(0, lastSlash) + "/";
        }


        lastSlash = src_path.find_last_of('/');
        if(lastSlash != 0) {
            source_parent = src_path.substr(0, lastSlash) + "/";
        }

        if( !remove_file(target_path) ) {
            return false; //cant remove
        }

        if( target_parent != source_parent && !move(src_path, target_parent) ){
           return false; //failed on move
        }

        File_entry*  parent_directory_file_entry = findFile(target_parent + source_file_name);
        if(parent_directory_file_entry  == 0) {
            return false; //no parent dir??
        }

        strcpy(parent_directory_file_entry->filename, target_file_name.c_str());  // rename source file

        return true;
    }




    size_t target_last_slash = target_path.find_last_of('/');
    if(target_path[0] != '/' || target_last_slash == string::npos ) {
        return false; //wrong path
    }
    string target_parent = target_path.substr(0, target_last_slash);
    File_entry* target_parent_dir_file_entry = findFile(target_parent);
    if(target_parent_dir_file_entry == 0 && target_path != "/") {
        return false; //wrong path
    }


    if(!target_file_entry) {//target dir does not exist
        if((source_file_entry->attr &  FLAG_DIR)) { //exists and dir
                target_file_entry = forced_create_dir(target_path);
                Directory_iterator directory_iterator(
                            this, get_block(source_file_entry->first_block), true
                            );
                while(directory_iterator.hasNext()){
                    string name = directory_iterator.next()->name();
                    if(! move(src_path + "/" + name, target_path)){
                        return false;
                    }
                    Directory_iterator buf_file_entryr_iterator(
                                this, get_block(source_file_entry->first_block), true
                                );
                    directory_iterator = buf_file_entryr_iterator;
                }
                remove(src_path); //remove src file
                return true;

        } else {//file is not dir
            File_entry*  target_file_entry = create_file(target_path, true);
            if(!target_file_entry) {
                return false; // failed on creating
            }
            int source_block_number = source_file_entry->first_block;
            int target_block_number = target_file_entry->first_block;

            for(int i=0; i<source_file_entry->blocks; i++) {
                Block*  source_block = get_block(source_block_number);
                Block*  target_block = get_block(target_block_number);
                //replacing blocks
                memcpy(target_block->get_data(), source_block->get_data(), target_block->get_size());

                if(source_block->get_header()->next_block == -1) {
                    target_block->get_header()->next_block = -1; //source end -> fix target last block
                    break;
                }

                //targets blocks left

                int newblock = allocateBlock();
                if(newblock == -1) {
                    return false; //cant allocate new block
                }
                target_block->get_header()->next_block = newblock;
                source_block_number = source_block->get_header()->next_block;
                target_block_number = newblock;
            }

            int target_first_block = target_file_entry->first_block;

            //sync attrs
            target_file_entry->copy(source_file_entry);
            strcpy(target_file_entry->filename, target_path.substr(target_last_slash+1).c_str());
            target_file_entry->first_block = target_first_block;
            target_file_entry->last_modified = time(0);

            remove(src_path);
            return true;
        }

    } else {//target dir exist
        string target_dir_path = target_path
                +(target_path[target_path.size()-1] == '/' ? "" : "/")
                + src_path.substr(target_last_slash+1); //form directory path
        File_entry*  target_dir_file_entry = findFile(target_dir_path, true);


        if(target_dir_file_entry && target_dir_file_entry->attr & FLAG_DIR) {
            return false; //such directory exist
        }

        //both files with same name
        if(target_dir_file_entry && !(target_dir_file_entry->attr &  FLAG_DIR) && !(source_file_entry->attr & FLAG_DIR)) {
            return move(src_path, target_dir_path);
        }
    }

    // so we have directorys here....

    Directory_iterator target_directory_iterator(
                this, get_block(target_file_entry->first_block), false
                );

    target_file_entry = target_directory_iterator.current();
    while(target_directory_iterator.hasNext()
          && (target_file_entry->attr & EXIST_FLAG)) {
        target_file_entry = target_directory_iterator.next();
    }
    if(target_directory_iterator.atBorder()) {
        int new_dir_block = allocateBlock();
        if(new_dir_block == -1) {
            return false;
        }
        target_directory_iterator.currentBlock()->get_header()->next_block = new_dir_block;
        Block*  newblock = get_block(new_dir_block);
        newblock->get_header()->next_block = -1;
        target_file_entry = (File_entry*)newblock->get_data();//moving to last block
    } else if(!target_directory_iterator.hasNext()) {
        ++target_file_entry;
    }
    target_file_entry->copy(source_file_entry);
    target_directory_iterator.dirHeader()->files_in_dir++;
    parent_dir_header->files_in_dir--;
    source_file_entry->attr ^= EXIST_FLAG;
    return true;
}

bool File_system::copyFile(File_entry*  src_path, const string& target_path) {

    string taget_path_copy = target_path;
    if( target_path.size() > 1 && target_path[0] == '/' && target_path[1] == '/') {
        taget_path_copy = target_path.substr(1, target_path.size());//fix path
    }

    size_t last_target_slash = taget_path_copy.find_last_of('/');
    string target_file_name = taget_path_copy.substr(last_target_slash + 1);

    string target_file;
    if(src_path->filename == target_file_name) target_file = taget_path_copy;
    else target_file = taget_path_copy + "/" + src_path->name();
    File_entry*  target_file_entry = findFile(target_file, true);
    if(target_file_entry) {
        remove(target_file);//clear target file
    }

    target_file_entry = create_file(target_file, true);
    if(!target_file_entry) {
        return false; //failed on create file
    }

    int src_block_number = src_path->first_block;
    int target_block_number = target_file_entry->first_block;

    //fill target's blocks
    for(int i=0; i<src_path->blocks; i++) {
        Block*  src_block = get_block(src_block_number);
        Block*  target_block = get_block(target_block_number);
        memcpy(target_block->get_data(), src_block->get_data(), target_block->get_size());

        if(src_block->get_header()->next_block == -1) {
            target_block->get_header()->next_block = -1;
            break;
        }
        int newblock = allocateBlock();
        if(newblock == -1) {
            return false;
        }
        target_block->get_header()->next_block = newblock;
        src_block_number = src_block->get_header()->next_block;
        target_block_number = newblock;
    }

    int target_first_block = target_file_entry->first_block;
    //init args
    target_file_entry->copy(src_path);
    target_file_entry->first_block = target_first_block;
    target_file_entry->last_modified = time(0);
    return true;
}

bool File_system::copyDir(Block*  src_dir, const string& target_dir) {
    Directory_iterator directory_iterator(this, src_dir);
    File_entry*  curr_file_entry = directory_iterator.current();
    string target_dir_path = target_dir + "/" + curr_file_entry->name(); //add name of src
    File_entry*  target_file_entry = findFile(target_dir_path, true);
    if(!target_file_entry) {
        target_file_entry = create_dir(target_dir_path);
        if(!target_file_entry) {
            return false;
        }
    }
    
    while(directory_iterator.hasNext()) {
        curr_file_entry = directory_iterator.next();
        bool result = (curr_file_entry->attr &  FLAG_DIR)
                ? copyDir(get_block(curr_file_entry->first_block), target_dir_path) :
                  copyFile(curr_file_entry, target_dir_path);
        if(!result) {
            return false;
        }
    }
    return true;
}

bool File_system::copy(const string& src, const string& target_c) {

    if( target_c.size() >= src.size() && target_c.substr(0, src.size()) == src){
        return false;
    }

    string target = target_c;
    if( target.size() > 1 && target[target.size()-1] == '/') {
        target = target.substr(0, target.size()-1); //fix path
    }


    size_t last_src_slash = src.find_last_of('/');
    string src_parent_dir = src.substr(0, last_src_slash);

    if(src_parent_dir.empty()) src_parent_dir = "/";
    if(src_parent_dir == target) return true; //already there =)

    File_entry*  src_file_entry = findFile(src);
    if(!src_file_entry) {
        return false; // no file =(
    }

    File_entry*  target_file_entry = findFile(target, true);
    if(!target_file_entry) {
        target_file_entry = forced_create_dir(target); //create dir
    } else if (!(target_file_entry->attr&  FLAG_DIR)) {
        return false; // file created!? WTF???
    }

    if(src_file_entry->attr&  FLAG_DIR) { //so we copy from dir...
        size_t right_bound_of_path = string::npos;
        while(1) {
            size_t left_string_position = target.find_last_of('/', right_bound_of_path);
            if(left_string_position == string::npos) break; //tooo looong
            string dir_path = target.substr(0, left_string_position);
            if(dir_path.empty()) break; //that dir is empty

            if(dir_path == src) {
                flag = 1;
            }
            right_bound_of_path = left_string_position-1;
        }
    }

    bool res = (src_file_entry->attr &  FLAG_DIR)
            ? copyDir(get_block(src_file_entry->first_block), target) :
              copyFile(src_file_entry, target);
    return res;
}


bool File_system::directory_info(File_entry*  dir) {
    Directory_iterator directory_iterator(this, get_block(dir->first_block));
    cout << " files in dir :" << directory_iterator.dirHeader()->files_in_dir << "\nfiles:" << std::endl;
    File_entry*  file_entry = directory_iterator.current();
    while(directory_iterator.hasNext()) {
        file_entry = directory_iterator.next();
        string sep = strlen(file_entry->filename) > 8 ? "\t" : "\t\t";
        if(file_entry->attr &  FLAG_DIR) {
            cout << "dir:" << sep << file_entry->name() << std::endl;
        } else {
            cout << "file:" << sep << file_entry->info();
        }
    }
    return true;
}

bool File_system::fileInfo(const string& path) {
    File_entry*  f = findFile(path);
    if(!f) return false;//no file
    if(f->attr &  FLAG_DIR) {
        cout <<"path:  " << path <<"  ";
        return directory_info(f);
    } else cout << f->info();
    return true;
}



//modify super block
void File_system::writeSuperBlock() {
    int curBlockSize = Size_of_superblock - sizeof(Super_block_info);

    string block_path = getBlockPath(0);
    FILE*  block_file = fopen(block_path.c_str(), "wb");
    fwrite(&super_block, sizeof(Super_block_info), 1, block_file);
    if(super_block_length == 1) {
        fwrite(super_block.bitmap, sizeof(char), super_block.bitmap_size, block_file);
        stretchBlock(block_file);
    } else fwrite(super_block.bitmap, sizeof(char), curBlockSize, block_file);
    fclose(block_file);

    int mapWritten = curBlockSize;
    Block_header bh;
    for(int i=1; i<super_block_length; i++) {
        block_file = fopen(getBlockPath(i).c_str(), "wb");
        bh.next_block = i == super_block_length - 1? -1 : i+1;
        fwrite(&bh, sizeof(Block_header), 1, block_file);
        if(i != super_block_length - 1) fwrite(super_block.bitmap+mapWritten, sizeof(char), Size_of_superblock-sizeof(Block_header), block_file);
        else {
            fwrite(super_block.bitmap+mapWritten, sizeof(char), super_block.bitmap_size-mapWritten, block_file);
            stretchBlock(block_file);
        }
        mapWritten += Size_of_superblock-sizeof(Block_header);
        fclose(block_file);
    }
}

//finding config file
string File_system::config_path(const char*  rootPath) {
    FILE*  f = fopen("config", "r");
    if(f) {
        fclose(f);
        return string("config");
    }
    string p(rootPath);
    p.append(p[p.size()-1] == '/' ? "config" : "/config");
    f = fopen(p.c_str(), "r");
    if(f) {
        fclose(f);
        return p;
    }
    return string();
}

//mark reserved blocks
void File_system::reserveBlock(int n) {
    int p = n/8;
    int s = 7-n%8;
    super_block.bitmap[p] |= 1 << s;
}

//return number om free block
int File_system::allocateBlock() {
    for(int i=0; i<N; i++) {
        int p = i/8;
        int s = 7-i%8;
        if((super_block.bitmap[p] &  (1 << s)) == 0) {
            super_block.bitmap[p] |= 1 << s;
            Block*  b = get_block(i);
            b->get_header()->next_block = -1;
            return i;
        }
    }
    return -1;
}


void File_system::free_block(int n) {
    int p = n/8;
    int s = 7-n%8;
    super_block.bitmap[p] ^= 1 << s;
}

//check if block marked as used
bool File_system::isUsedBlock(int n) const {
    int p = n/8;
    int s = 7-n%8;
    return super_block.bitmap[p]&  1 << s;
}

//count free blocks in bitmap
size_t File_system::freeSpace() const {
    size_t sp = 0;
    for(int i=0; i<N; i++) {
        if(!isUsedBlock(i)) sp += Size_of_superblock;
    }
    return sp;
}

//return path of block n
string File_system::getBlockPath(int n) {
    char buf[10];
    string block_name = path;
    sprintf(buf, "%d", n);
    block_name.append(buf);
    return block_name;
}

//print bitmap
void File_system::print_bitmap() {
    for(int i=0; i<N; i++) {
        int p = i/8;
        int s = 7-i%8;
        if(super_block.bitmap[p]&  (1 << s)) {
            cout << '1';
        } else {
            cout << '0';
        }
    }
    cout <<std::endl;
}

//stretch super_block size if not enough space to keep info
void File_system::stretchBlock(FILE*  block) {
    char a = 0;
    fseek(block, Size_of_superblock, SEEK_SET);
    fwrite(&a, sizeof(char), 1, block);
}

// ---

File_entry*  File_system::findFile(const string& path, bool test) {

    if(path == "/" || path.empty()) {
        return &root;
    }
    if(path[0] != '/') {
        return 0;
    }
    string rpath = path[path.size()-1] == '/' ? path.substr(0, path.size()-1) : path;
    //remove slash from end

    size_t current_position = 1;
    File_entry*  curDir = &root;
    while(1) {
        size_t second_position = rpath.find_first_of('/', current_position);
        string directory_name = second_position != string::npos ? rpath.substr(current_position, second_position-current_position) : rpath.substr(current_position);
        if(directory_name.size() >NAME_BOUND) {
            return 0;//tooo loooong
        }

        bool found = false;
        Directory_iterator directory_iterator(this, get_block(curDir->first_block));
        File_entry*  file_entry = directory_iterator.current();
        while(directory_iterator.hasNext()) {
            file_entry = directory_iterator.next();
            if(file_entry->name() == directory_name) {
                if(second_position == string::npos) return file_entry;
                else if(! (file_entry->attr & FLAG_DIR) ) {
                    return 0;//cant move deeper
                }
                found = true;
                curDir = file_entry;
                break;
            }
        }

        if(!found) {
            return 0;
        }

        current_position = second_position+1;
    }
    return 0;
}

