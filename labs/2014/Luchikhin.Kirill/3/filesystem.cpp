#include "filesystem.h"

//------------------------------------------------------------------

Block::Block(size_t i, size_t s) {
    bindex = i;
    bsize = s - sizeof(BlockHeader);
    bdata = new char[bsize];
}

Block::Block(size_t i, size_t s, const char *bn) {
    bindex = i;
    bsize = s - sizeof(BlockHeader);
    bdata = new char[bsize];
    read(bn);
}

Block::~Block() {
    //delete[] bdata;
}

void Block::read(const char *bn) {
    FILE *blockFile = fopen(bn, "rb");
    fread(&bheader, sizeof(BlockHeader), 1, blockFile);
    fread(bdata, sizeof(char), bsize, blockFile);
    fclose(blockFile);
}

void Block::save(const char *bn) {
    FILE *blockFile = fopen(bn, "wb");
    fwrite(&bheader, sizeof(BlockHeader), 1, blockFile);
    fwrite(bdata, sizeof(char), bsize, blockFile);
    fclose(blockFile);
}

/****************************************************************************/

BlockBuffer::BlockBuffer(size_t s, const std::string &p) : bsize(s), path(p) {
    if(path[path.size()-1] != '/') path.append("/");
}

BlockBuffer::~BlockBuffer() {
}

Block *BlockBuffer::getBlock(size_t n) {
    std::map<size_t, Block>::iterator i = blocks.find(n);
    if(i == blocks.end()) {
        std::stringstream s; s << path << n;
        std::pair<std::map<size_t, Block>::iterator, bool> pb = blocks.insert(std::pair<int, Block>(n, Block(n, bsize, s.str().c_str())));
        return &(pb.first->second);
    } else {
        return &(i->second);
    }
    return 0;
}

void BlockBuffer::save() {
    for(std::map<size_t, Block>::iterator i = blocks.begin(); i != blocks.end(); ++i) {
        std::stringstream s; s << path << i->first;
        i->second.save(s.str().c_str());
    }
}

/****************************************************************************/

FileSystem::FileSystem(const char *rootPath) : ready(false), S(0), N(0), path(rootPath), blocks(0) {
    if(path[path.size()-1] != '/') path.append("/");
    std::string config = configPath(rootPath);
    if(!config.empty()) {
        std::ifstream configFile(config.c_str());
        if(!configFile.is_open()) {
            std::cout << "ERROR: Unable to init filesystem: unable to open config file" << std::endl;
            return;
        }
        configFile >> S >> N;
        configFile.close();
    } else {
        std::string sblockPath(rootPath);
        sblockPath.append(sblockPath[sblockPath.size()-1] == '/' ? "0" : "/0");
        FILE *sblockFile = fopen(sblockPath.c_str(), "rb");
        if(!sblockFile) {
            std::cout << "ERROR: Unable to init filesystem: config file not found" << std::endl;
            return;
        }
        SuperBlock tmpSBlock;
        fread(&tmpSBlock, sizeof(SuperBlock), 1, sblockFile);
        S = tmpSBlock.bsize;
        N = tmpSBlock.bcount;
        fclose(sblockFile);
    }

    blocks = new BlockBuffer(S, path);

    ready = true;
}

FileSystem::~FileSystem() {
    delete blocks;
}

bool FileSystem::init() {
    if(!ready) return false;
    char buf[10];
    for(int i=0; i<N; i++) {
        std::string blockName = path;
        sprintf(buf, "%d", i);
        blockName.append(buf);
        std::ofstream bf(blockName.c_str(), std::ios_base::out | std::ios_base::binary);
        if(!bf.is_open()) {
            std::cout << "ERROR: Unable to create blocks" << std::endl;
            return false;
        }
        bf.close();
    }
    return true;
}

bool FileSystem::format() {
    if(!ready) return false;

    sBlock.bcount = N;
    sBlock.bsize = S;
    sBlock.mapSize = N/8+1;
    sBlock.map = new char[sBlock.mapSize];
    memset(sBlock.map, 0, sizeof(char)*sBlock.mapSize);
    int curBlockSize = S - sizeof(SuperBlock);
    sBlockLength = sBlock.mapSize > curBlockSize ? (sBlock.mapSize-curBlockSize)/(S-sizeof(BlockHeader))+2 : 1;
    sBlock.bnext = sBlockLength > 1 ? 1 : -1;
    sBlock.broot = sBlockLength;
    for(int i=0; i<=sBlockLength; i++) reserveBlock(i);

    writeSuperBlock();

    FILE *blockFile = fopen(getBlockPath(sBlockLength).c_str(), "wb");
    BlockHeader bh;
    bh.next = -1;
    fwrite(&bh, sizeof(BlockHeader), 1, blockFile);

    DirHeader dh;
    dh.fileCount = 0;
    fwrite(&dh, sizeof(DirHeader), 1, blockFile);

    root.attr = FA_DIR | FA_USED;
    root.blocks = 1;
    root.filename[0] = '/';
    root.firstBlock = sBlockLength;
    root.modTime = time(0);
    root.size = sizeof(FileEntry);
    fwrite(&root, sizeof(FileEntry), 1, blockFile);
    fclose(blockFile);

    return true;
}

bool FileSystem::load() {
    if(!ready) return false;

    FILE *blockFile = fopen(getBlockPath(0).c_str(), "rb");
    fread(&sBlock, sizeof(SuperBlock), 1, blockFile);
    sBlock.map = new char[sBlock.mapSize];
    sBlockLength = 1;
    if(sBlock.bnext == -1) {
        fread(sBlock.map, sizeof(char), sBlock.mapSize, blockFile);
        fclose(blockFile);
    } else {
        fread(sBlock.map, sizeof(char), S-sizeof(SuperBlock), blockFile);
        fclose(blockFile);
        BlockHeader bh;
        bh.next = sBlock.bnext;
        int mapWritten = S-sizeof(SuperBlock);
        while (bh.next != -1) {
            blockFile = fopen(getBlockPath(bh.next).c_str(), "rb");
            fread(&bh, sizeof(BlockHeader), 1, blockFile);
            if(bh.next != -1) fread(sBlock.map+mapWritten, sizeof(char), S-sizeof(BlockHeader), blockFile);
            else fread(sBlock.map+mapWritten, sizeof(char), sBlock.mapSize-mapWritten, blockFile);
            fclose(blockFile);
            mapWritten += S-sizeof(BlockHeader);
            sBlockLength++;
        }
    }

    BlockHeader bh;
    DirHeader dh;
    blockFile = fopen(getBlockPath(sBlock.broot).c_str(), "rb");
    fread(&bh, sizeof(BlockHeader), 1, blockFile);
    fread(&dh, sizeof(DirHeader), 1, blockFile);
    fread(&root, sizeof(FileEntry), 1, blockFile);
    fclose(blockFile);

    blocks->getBlock(sBlock.broot);

//    for(int i=0; i<N; i++) {
//        blocks.push_back(Block(i, S, getBlockPath(i).c_str()));
//    }

    return true;
}

bool FileSystem::save() {
    if(!ready) return false;

    writeSuperBlock();
    blocks->save();
//    for(int i=sBlockLength; i<N; i++) {
//        if(isUsedBlock(i)) blocks[i].save(getBlockPath(i).c_str());
//    }

    return true;
}

//------------------------------------------------------------------

FileEntry *FileSystem::createFile(const std::string &path, bool alloc) {
    size_t lastSlash = path.find_last_of('/');
    if(lastSlash == std::string::npos || lastSlash == path.size()-1 ) {
        std::cout << "Unable to create file: wrong path" << std::endl;
        return 0;
    }

    std::string fname = path.substr(lastSlash+1);
    if(fname.size() > MAX_NAME_LENGTH) {
        std::cout << "Unable to create file: filename is too long" << std::endl;
        return 0;
    }

    FileEntry *oldf = findFile(path, true);
    if(oldf != 0) return oldf;

    FileEntry *dir = findFile(path.substr(0, lastSlash));
    if(!dir) return 0;
    if(!(dir->attr & FA_DIR)) {
        std::cout << "Unable to create file: " << path <<". Path is not a directory" << std::endl;
        return 0;
    }

    DirIterator dit(this, getBlock(dir->firstBlock), false);
    FileEntry *fe = dit.current();
    while(dit.hasNext() && fe->attr & FA_USED) {
        fe = dit.next();
    }
    if(dit.atBorder()) {
        int inewblock = allocateBlock();
        if(inewblock == -1) {
            std::cout << "Unable to create file: not enough space" << std::endl;
            return 0;
        }
        dit.currentBlock()->header()->next = inewblock;
        Block *newblock = getBlock(inewblock);
        newblock->header()->next = -1;
        fe = (FileEntry*)newblock->data();
    } else {
        ++fe;
    }

    fe->attr = FA_USED;
    fe->blocks = alloc ? 1 : 0;
    fe->size = 0;
    strncpy(fe->filename, fname.c_str(), MAX_NAME_LENGTH);
    fe->modTime = time(0);
    fe->firstBlock = alloc ? allocateBlock() : -1;

    if(alloc && fe->firstBlock == -1) {
        std::cout << "Unable to create file: not enough space" << std::endl;
        return 0;
    }

    dit.dirHeader()->fileCount++;
    return fe;
}

FileEntry *FileSystem::forcedCreateFile(const std::string &path, bool alloc) {
    size_t lastSlash = path.find_last_of('/');
    if(path[0] != '/' || lastSlash == std::string::npos || lastSlash == path.size()-1 ) {
        std::cout << "Unable to create file: wrong path" << std::endl;
        return 0;
    }

    FileEntry *oldf = findFile(path, true);
    if(oldf != 0) return oldf;

    size_t cpos = 1;
    FileEntry *curDir = &root;
    while(1) {
        size_t spos = path.find_first_of('/', cpos);
        std::string ndir = spos != std::string::npos ? path.substr(cpos, spos-cpos) : path.substr(cpos);
        if(spos == std::string::npos) break;

        if(ndir.size() > MAX_NAME_LENGTH) {
            std::cout << "Unable to create file: file name is too long: " << path << std::endl;
            return 0;
        }

        curDir = findFile(path.substr(0, spos), true);
        if(!curDir) curDir = createDir(path.substr(0, spos));
        cpos = spos+1;
    }

    return createFile(path, alloc);
}

FileEntry *FileSystem::createDir(const std::string &path) {
    FileEntry *dir = createFile(path, true);
    if(!dir) return 0;

    dir->attr |= FA_DIR;
    DirHeader *dh = (DirHeader*)getBlock(dir->firstBlock)->data();
    dh->fileCount = 0;
    FileEntry *ndir = (FileEntry*)(getBlock(dir->firstBlock)->data()+sizeof(DirHeader));
    ndir->copy(dir);
    return dir;
}

FileEntry *FileSystem::forcedCreateDir(const std::string &path) {
    size_t lastSlash = path.find_last_of('/');
    if(path[0] != '/' || lastSlash == std::string::npos || lastSlash == path.size()-1 ) {
        std::cout << "Unable to create dir: wrong path" << std::endl;
        return 0;
    }

    FileEntry *oldf = findFile(path, true);
    if(oldf != 0) return oldf;

    size_t cpos = 1;
    FileEntry *curDir = &root;
    while(1) {
        size_t spos = path.find_first_of('/', cpos);
        std::string ndir = spos != std::string::npos ? path.substr(cpos, spos-cpos) : path.substr(cpos);
        if(spos == std::string::npos) break;

        if(ndir.size() > MAX_NAME_LENGTH) {
            std::cout << "Unable to create dir: file name is too long: " << path << std::endl;
            return 0;
        }

        curDir = findFile(path.substr(0, spos), true);
        if(!curDir) curDir = createDir(path.substr(0, spos));
        cpos = spos+1;
    }

    return createDir(path);
}

//------------------------------------------------------------------

bool FileSystem::importFile(const std::string &src, const std::string &dst) {
    FILE *srcFile = fopen(src.c_str(), "rb");
    if(!srcFile) {
        std::cout << "Unable to import: source file " << src << " not found" << std::endl;
        return false;
    }

    fseek(srcFile, 0, SEEK_END);
    size_t sz = ftell(srcFile);

    if(sz > freeSpace()) {
        std::cout << "Unable to import: not enough space" << std::endl;
        return false;
    }

    fseek(srcFile, 0, SEEK_SET);
    char *buf = new char[S];


    FileEntry *destFile = findFile(dst, true);
    if(destFile) {
        removeFile(dst);
    }
    destFile = forcedCreateFile(dst, true);
    if(!destFile) return false;

    Block *curBlock = getBlock(destFile->firstBlock);
    size_t brc = 0;
    size_t blockCount = 1;
    while(true) {
        size_t br = fread(buf, sizeof(char), S-sizeof(BlockHeader), srcFile);
        memcpy(curBlock->data(), buf, curBlock->size());
        brc += br;
        if(brc >= sz) {
            curBlock->header()->next = -1;
            break;
        }
        int newblock = allocateBlock();
        curBlock->header()->next = newblock;
        curBlock = getBlock(newblock);
        blockCount++;
    }
    fclose(srcFile);

    destFile->blocks = blockCount;
    destFile->size = sz;
    destFile->modTime = time(0);
    return true;
}

bool FileSystem::exportFile(const std::string &src, const std::string &dst) {
    FileEntry *srcFile = findFile(src);
    if(!srcFile || srcFile->attr & FA_DIR) return false;

    FILE *dstFile = fopen(dst.c_str(), "wb");
    size_t sizeleft = srcFile->size;
    Block *curBlock = getBlock(srcFile->firstBlock);
    for(int i=0; i<srcFile->blocks; i++) {
        if(sizeleft > curBlock->size()) {
            fwrite(curBlock->data(), sizeof(char), curBlock->size(), dstFile);
            sizeleft -= curBlock->size();
            curBlock = getBlock(curBlock->header()->next);
        } else {
            fwrite(curBlock->data(), sizeof(char), sizeleft, dstFile);
            break;
        }
    }
    fclose(dstFile);
    return true;
}

//------------------------------------------------------------------

bool FileSystem::removeFile(const std::string &path, bool recursive) {
    FileEntry *fe = findFile(path);
    if(!fe || (fe->attr & FA_DIR)) return false;

    size_t lastSlash = path.find_last_of('/');
    FileEntry *pdir = findFile(path.substr(0, lastSlash));
    DirHeader *pdh = (DirHeader*)getBlock(pdir->firstBlock)->data();

    int cur = fe->firstBlock;
    for(int i=0; i<fe->blocks; i++) {
        int next = getBlock(cur)->header()->next;
        freeBlock(cur);
        if(next == -1) break;
        cur = next;
    }

    fe->attr ^= FA_USED;
    if(!recursive) pdh->fileCount--;

    return true;
}

bool FileSystem::remove(const std::string &path, bool recursive) {
    if(path == "/") return false; //need format?
    FileEntry *fe = findFile(path);
    if(!fe) return false;
    if(!(fe->attr & FA_DIR)) return removeFile(path, recursive);

    size_t lastSlash = path.find_last_of('/');
    FileEntry *pdir = findFile(path.substr(0, lastSlash));
    DirHeader *pdh = (DirHeader*)getBlock(pdir->firstBlock)->data();

    DirIterator dit(this, getBlock(fe->firstBlock));
    FileEntry *cdfe = dit.current();
    while(dit.hasNext()) {
        cdfe = dit.next();
        std::stringstream s; s << path << "/" << cdfe->name();
        remove(s.str(), true);
    }

    int cur = fe->firstBlock;
    while(true) {
        if(cur == -1) break;
        int next = getBlock(cur)->header()->next;
        freeBlock(cur);
        cur = next;
    }

    fe->attr ^= FA_USED;
    if(!recursive) pdh->fileCount--;

    return true;
}

//------------------------------------------------------------------

bool FileSystem::move(const std::string &src, const std::string &dst) {
    if(src == dst) return false;

    FileEntry *srcFE = findFile(src);
    if(!srcFE) return false;

    size_t lastSlash = src.find_last_of('/');
    FileEntry *pdir = findFile(src.substr(0, lastSlash));
    DirHeader *pdh = (DirHeader*)getBlock(pdir->firstBlock)->data();

    FileEntry *dstFE = findFile(dst);
    if(!dstFE) dstFE = forcedCreateDir(dst);
    else {
        std::string testDst = dst + (dst[dst.size()-1] == '/' ? "" : "/") + src.substr(lastSlash+1);
        FileEntry *testDstFE = findFile(testDst, true);
        if(testDstFE) {
            std::cout << "ERROR: Unable to move: source file name already exists in destination path" << std::endl;
            return false;
        }
    }
    if(!(dstFE->attr & FA_DIR)) {
        std::cout << "ERROR: Unable to move: destination is not a directory" << std::endl;
        return false;
    }


    DirIterator dit(this, getBlock(dstFE->firstBlock), false);
    dstFE = dit.current();
    while(dit.hasNext() && dstFE->attr & FA_USED) dstFE = dit.next();
    if(dit.atBorder()) {
        int inewblock = allocateBlock();
        if(inewblock == -1) {
            std::cout << "Unable to move: unable to allocate block in destination folder" << std::endl;
            return false;
        }
        dit.currentBlock()->header()->next = inewblock;
        Block *newblock = getBlock(inewblock);
        newblock->header()->next = -1;
        dstFE = (FileEntry*)newblock->data();
    } else if(!dit.hasNext()) ++dstFE;

    dstFE->copy(srcFE);
    dit.dirHeader()->fileCount++;
    pdh->fileCount--;
    srcFE->attr ^= FA_USED;
    return true;
}

//------------------------------------------------------------------

bool FileSystem::copyFile(FileEntry *src, const std::string &dst) {
    std::string dstFile = dst + "/" + src->name();
    FileEntry *testFE = findFile(dstFile, true);
    if(testFE) {
        remove(dstFile);
        //std::cout << "WARNING: Unable to copy file: file already exists" << std::endl;
        //return false;
    }

    FileEntry *copyFE = createFile(dstFile, true);
    if(!copyFE) return false;

    int srcBlock = src->firstBlock;
    int dstBlock = copyFE->firstBlock;

    for(int i=0; i<src->blocks; i++) {
        Block *sblock = getBlock(srcBlock);
        Block *dblock = getBlock(dstBlock);
        memcpy(dblock->data(), sblock->data(), dblock->size());

        if(sblock->header()->next == -1) {
            dblock->header()->next = -1;
            break;
        }
        int newblock = allocateBlock();
        if(newblock == -1) {
            std::cout << freeSpace() << std::endl;
            std::cout << "Unable to allocate block" << std::endl;
            return false;
        }
        dblock->header()->next = newblock;
        srcBlock = sblock->header()->next;
        dstBlock = newblock;
    }

    int fb = copyFE->firstBlock;
    copyFE->copy(src);
    copyFE->firstBlock = fb;
    copyFE->modTime = time(0);
    return true;
}

bool FileSystem::copyDir(Block *src, const std::string &dst) {
    DirIterator dit(this, src);
    FileEntry *curFE = dit.current();
    std::string dstDirPath = dst + "/" + curFE->name();
    FileEntry *dstFE = findFile(dstDirPath, true);
    if(!dstFE) {
		dstFE = createDir(dstDirPath);
		if(!dstFE) return false;
    }
    
    while(dit.hasNext()) {
        curFE = dit.next();
        bool res = (curFE->attr & FA_DIR) ? copyDir(getBlock(curFE->firstBlock), dstDirPath) : copyFile(curFE, dstDirPath);
        if(!res) return false;
    }
    return true;
}

bool FileSystem::copy(const std::string &src, const std::string &dst) {
    if(src == dst) {
        std::cout << "Unable to copy: source and destination paths are same" << std::endl;
        return false;
    }

    size_t lastSlash = src.find_last_of('/');
    std::string ppath = src.substr(0, lastSlash);
    if(ppath.empty()) ppath = "/";
    if(ppath == dst) return true;

    FileEntry *srcFE = findFile(src);
    if(!srcFE) return false;

    FileEntry *dstFE = findFile(dst, true);
    if(!dstFE) dstFE = forcedCreateDir(dst);
    else if(!(dstFE->attr & FA_DIR)) {
        std::cout << "Unable to copy: destination path is not a directory" << std::endl;
        return false;
    }

    if(srcFE->attr & FA_DIR) {
        size_t epos = std::string::npos;
        while(1) {
            size_t lspos = dst.find_last_of('/', epos);
            if(lspos == std::string::npos) break;
            std::string dpath = dst.substr(0, lspos);
            if(dpath.empty()) break;
            if(dpath == src) {
                std::cout << "Unable to copy: the destination folder is inside the source folder" << std::endl;
                return false;
            }
            epos = lspos-1;
        }
    }

    bool res = (srcFE->attr & FA_DIR) ? copyDir(getBlock(srcFE->firstBlock), dst) : copyFile(srcFE, dst);
    return res;
}

//------------------------------------------------------------------

bool FileSystem::viewDir(const std::string &path) {
    FileEntry *dir = findFile(path);
    if(dir && (dir->attr & FA_DIR)) {
        std::cout << path << ":" << std::endl;
        return viewDir(dir);
    }
    return false;
}

bool FileSystem::viewDir(FileEntry *dir) {
    DirIterator dit(this, getBlock(dir->firstBlock));
    FileEntry *fe = dit.current();
    while(dit.hasNext()) {
        fe = dit.next();
        std::string sep = strlen(fe->filename) > 8 ? "\t" : "\t\t";
        std::cout << fe->name() << sep << ((fe->attr & FA_DIR) ? "<dir>" : "<file>") << std::endl;
    }
    if(dit.dirHeader()->fileCount == 0) std::cout << "<empty>" << std::endl;
    return true;
}

bool FileSystem::fileInfo(const std::string &path) {
    FileEntry *f = findFile(path);
    if(!f) return false;
    if(f->attr & FA_DIR) {
        std::cout << path << ":" << std::endl;
        return viewDir(f);
    } else std::cout << f->info() << std::endl;
    return true;
}

//------------------------------------------------------------------

void FileSystem::writeSuperBlock() {
    int curBlockSize = S - sizeof(SuperBlock);

    std::string blockPath = getBlockPath(0);
    FILE *blockFile = fopen(blockPath.c_str(), "wb");
    fwrite(&sBlock, sizeof(SuperBlock), 1, blockFile);
    if(sBlockLength == 1) {
        fwrite(sBlock.map, sizeof(char), sBlock.mapSize, blockFile);
        stretchBlock(blockFile);
    } else fwrite(sBlock.map, sizeof(char), curBlockSize, blockFile);
    fclose(blockFile);

    int mapWritten = curBlockSize;
    BlockHeader bh;
    for(int i=1; i<sBlockLength; i++) {
        blockFile = fopen(getBlockPath(i).c_str(), "wb");
        bh.next = i == sBlockLength - 1? -1 : i+1;
        fwrite(&bh, sizeof(BlockHeader), 1, blockFile);
        if(i != sBlockLength - 1) fwrite(sBlock.map+mapWritten, sizeof(char), S-sizeof(BlockHeader), blockFile);
        else {
            fwrite(sBlock.map+mapWritten, sizeof(char), sBlock.mapSize-mapWritten, blockFile);
            stretchBlock(blockFile);
        }
        mapWritten += S-sizeof(BlockHeader);
        fclose(blockFile);
    }
}

//------------------------------------------------------------------

std::string FileSystem::configPath(const char *rootPath) {
    FILE *f = fopen("config", "r");
    if(f) {
        fclose(f);
        return std::string("config");
    }
    std::string p(rootPath);
    p.append(p[p.size()-1] == '/' ? "config" : "/config");
    f = fopen(p.c_str(), "r");
    if(f) {
        fclose(f);
        return p;
    }
    return std::string();
}

void FileSystem::reserveBlock(int n) {
    int p = n/8;
    int s = 7-n%8;
    sBlock.map[p] |= 1 << s;
}

int FileSystem::allocateBlock() {
    for(int i=0; i<N; i++) {
        int p = i/8;
        int s = 7-i%8;
        if((sBlock.map[p] & (1 << s)) == 0) {
            sBlock.map[p] |= 1 << s;
            Block *b = getBlock(i);
            b->header()->next = -1;
            return i;
        }
    }
    return -1;
}

void FileSystem::freeBlock(int n) {
    int p = n/8;
    int s = 7-n%8;
    sBlock.map[p] ^= 1 << s;
}

bool FileSystem::isUsedBlock(int n) const {
    int p = n/8;
    int s = 7-n%8;
    return sBlock.map[p] & 1 << s;
}

size_t FileSystem::freeSpace() const {
    size_t sp = 0;
    for(int i=0; i<N; i++) {
        if(!isUsedBlock(i)) sp += S;
    }
    return sp;
}

std::string FileSystem::getBlockPath(int n) {
    char buf[10];
    std::string blockName = path;
    sprintf(buf, "%d", n);
    blockName.append(buf);
    return blockName;
}

void FileSystem::printMap() {
    for(int i=0; i<N; i++) {
        int p = i/8;
        int s = 7-i%8;
        if(sBlock.map[p] & (1 << s)) std::cout << '1';
        else std::cout << '0';
    }
    std::cout << std::endl;
}

void FileSystem::stretchBlock(FILE *block) {
    char a = 0;
    fseek(block, S, SEEK_SET);
    fwrite(&a, sizeof(char), 1, block);
}

//------------------------------------------------------------------

FileEntry *FileSystem::findFile(const std::string &path, bool test) {
    if(path == "/" || path.empty()) return &root;
    if(path[0] != '/') {
        if(!test) std::cout << "Unable to find file: invalid path: " << path << std::endl;
        return 0;
    }
    std::string rpath = path[path.size()-1] == '/' ? path.substr(0, path.size()-1) : path;

    size_t cpos = 1;
    FileEntry *curDir = &root;
    while(1) {
        size_t spos = rpath.find_first_of('/', cpos);
        std::string ndir = spos != std::string::npos ? rpath.substr(cpos, spos-cpos) : rpath.substr(cpos);
        if(ndir.size() > MAX_NAME_LENGTH) {
            if(!test) std::cout << "Unable to find file: file name is too long: " << path << std::endl;
            return 0;
        }

        bool found = false;
        DirIterator dit(this, getBlock(curDir->firstBlock));
        FileEntry *fe = dit.current();
        while(dit.hasNext()) {
            fe = dit.next();
            if(fe->name() == ndir) {
                if(spos == std::string::npos) return fe;
                else if(! (fe->attr & FA_DIR) ) {
                    if(!test) std::cout << "Unable to find file: path element is not a directory: " << path << std::endl;
                    return 0;
                }
                found = true;
                curDir = fe;
                break;
            }
        }

        if(!found) {
            if(!test) std::cout << "Unable to find file: path not exists: " << path << std::endl;
            return 0;
        }

        cpos = spos+1;
    }
    return 0;
}

/****************************************************************************/

DirIterator::DirIterator(FileSystem *fsys, Block *b, bool skipEmpty) {
    curBlock = b;
    dh = (DirHeader*)b->data();
    curFE = (FileEntry*)(b->data()+sizeof(DirHeader));
    pos = 0;
    skip = skipEmpty;
    fs = fsys;
}

bool DirIterator::hasNext() const {
    return pos < dh->fileCount;
}

FileEntry *DirIterator::current() {
    return curFE;
}

FileEntry *DirIterator::next() {
    while(1) {
        ++curFE;
        if((char*)curFE+sizeof(FileEntry) > curBlock->data()+curBlock->size()) {
            curBlock = fs->getBlock(curBlock->header()->next);
            curFE = (FileEntry*)curBlock->data();
        }
        if(curFE->attr & FA_USED) {
            pos++;
            break;
        } else if(!skip) break;
    }
    return curFE;
}

bool DirIterator::atBorder() const {
    return ((char*)curFE+2*sizeof(FileEntry) > curBlock->data()+curBlock->size());
}

Block *DirIterator::currentBlock() {
    return curBlock;
}

DirHeader *DirIterator::dirHeader() {
    return dh;
}
