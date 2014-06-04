#include "filesystem.hpp"


#include <cstring>
#include <string>

Block::Block(size_t i, size_t s) {
    blockIndex = i;
    blockSize = s - sizeof(HeaderForBlock);
    blockData = new char[blockSize];
}

Block::Block(size_t i, size_t s, const char *bn) {
    blockIndex = i;
    blockSize = s - sizeof(HeaderForBlock);
    blockData = new char[blockSize];
    read(bn);
}

Block::~Block() {
}

void Block::read(const char *bn) {
    FILE *blockFile = fopen(bn, "rb");
    fread(&blockHeader, sizeof(HeaderForBlock), 1, blockFile);
    fread(blockData, sizeof(char), blockSize, blockFile);
    fclose(blockFile);
}

void Block::save(const char *bn) {
    FILE *blockFile = fopen(bn, "wb");
    fwrite(&blockHeader, sizeof(HeaderForBlock), 1, blockFile);
    fwrite(blockData, sizeof(char), blockSize, blockFile);
    fclose(blockFile);
}

/////////////

BlockContainer::BlockContainer(size_t s, const std::string &p) : blockSize(s), path(p) {
    if(path[path.size()-1] != '/') path.append("/");
}

BlockContainer::~BlockContainer() {
}

Block *BlockContainer::getBlock(size_t n) {
    std::map<size_t, Block>::iterator i = blocks.find(n);
    if(i == blocks.end()) {
        std::stringstream s; s << path << n;
        std::pair<std::map<size_t, Block>::iterator, bool> pb = blocks.insert(std::pair<int, Block>(n, Block(n, blockSize, s.str().c_str())));
        return &(pb.first->second);
    } else {
        return &(i->second);
    }
    return 0;
}

void BlockContainer::save() {
    for(std::map<size_t, Block>::iterator i = blocks.begin(); i != blocks.end(); ++i) {
        std::stringstream s; s << path << i->first;
        i->second.save(s.str().c_str());
    }
}

/////////////

FileSystem::FileSystem(const char *rootPath) : ready(false), S(0), N(0), path(rootPath), blocks(0) {
    if(path[path.size()-1] != '/') path.append("/");
    std::string config = configPath(rootPath);
    if(!config.empty()) {
        std::ifstream configFile(config.c_str());
        if(!configFile.is_open()) {
            return;
        }
        configFile >> S >> N;
        configFile.close();
    } else {
        std::string sblockPath(rootPath);
        sblockPath.append(sblockPath[sblockPath.size()-1] == '/' ? "0" : "/0");
        FILE *sblockFile = fopen(sblockPath.c_str(), "rb");
        if(!sblockFile) {
            return;
        }
        InfoForSuperBlock tmpSBlock;
        fread(&tmpSBlock, sizeof(InfoForSuperBlock), 1, sblockFile);
        S = tmpSBlock.blockSize;
        N = tmpSBlock.blockCount;
        fclose(sblockFile);
    }
    flag = 1;
    blocks = new BlockContainer(S, path);

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
            return false;
        }
        bf.close();
    }
    return true;
}

bool FileSystem::format() {
    if(!ready) return false;

    sBlock.blockCount = N;
    sBlock.blockSize = S;
    sBlock.mapSize = N/8+1;
    sBlock.map = new char[sBlock.mapSize];
    memset(sBlock.map, 0, sizeof(char)*sBlock.mapSize);
    int curBlockSize = S - sizeof(InfoForSuperBlock);
    sBlockLength = sBlock.mapSize > curBlockSize ? (sBlock.mapSize-curBlockSize)/(S-sizeof(HeaderForBlock))+2 : 1;
    sBlock.blockNext = sBlockLength > 1 ? 1 : -1;
    sBlock.blockRoot = sBlockLength;
    for(int i=0; i<=sBlockLength; i++) reserveBlock(i);

    writeSuperBlock();

    FILE *blockFile = fopen(getBlockPath(sBlockLength).c_str(), "wb");
    HeaderForBlock bh;
    bh.next = -1;
    fwrite(&bh, sizeof(HeaderForBlock), 1, blockFile);

    HeaderForDirectory dh;
    dh.fileCount = 0;
    fwrite(&dh, sizeof(HeaderForDirectory), 1, blockFile);

    root.attr = FLAG_DIR | FLAG;
    root.blocks = 1;
    root.filename[0] = '/';
    root.firstBlock = sBlockLength;
    root.modTime = time(0);
    root.size = sizeof(EntryForFile);
    fwrite(&root, sizeof(EntryForFile), 1, blockFile);
    fclose(blockFile);

    return true;
}

bool FileSystem::load() {
    if(!ready) return false;

    FILE *blockFile = fopen(getBlockPath(0).c_str(), "rb");
    fread(&sBlock, sizeof(InfoForSuperBlock), 1, blockFile);
    sBlock.map = new char[sBlock.mapSize];
    sBlockLength = 1;
    if(sBlock.blockNext == -1) {
        fread(sBlock.map, sizeof(char), sBlock.mapSize, blockFile);
        fclose(blockFile);
    } else {
        fread(sBlock.map, sizeof(char), S-sizeof(InfoForSuperBlock), blockFile);
        fclose(blockFile);
        HeaderForBlock bh;
        bh.next = sBlock.blockNext;
        int mapWritten = S-sizeof(InfoForSuperBlock);
        while (bh.next != -1) {
            blockFile = fopen(getBlockPath(bh.next).c_str(), "rb");
            fread(&bh, sizeof(HeaderForBlock), 1, blockFile);
            if(bh.next != -1) fread(sBlock.map+mapWritten, sizeof(char), S-sizeof(HeaderForBlock), blockFile);
            else fread(sBlock.map+mapWritten, sizeof(char), sBlock.mapSize-mapWritten, blockFile);
            fclose(blockFile);
            mapWritten += S-sizeof(HeaderForBlock);
            sBlockLength++;
        }
    }

    HeaderForBlock bh;
    HeaderForDirectory dh;
    blockFile = fopen(getBlockPath(sBlock.blockRoot).c_str(), "rb");
    fread(&bh, sizeof(HeaderForBlock), 1, blockFile);
    fread(&dh, sizeof(HeaderForDirectory), 1, blockFile);
    fread(&root, sizeof(EntryForFile), 1, blockFile);
    fclose(blockFile);

    blocks->getBlock(sBlock.blockRoot);

    return true;
}

bool FileSystem::save() {
    if(!ready) return false;

    writeSuperBlock();
    blocks->save();

    return true;
}

// ---

EntryForFile *FileSystem::createFile(const std::string &path, bool alloc) {
    size_t lastSlash = path.find_last_of('/');
    if(lastSlash == std::string::npos || lastSlash == path.size()-1 ) {
        return 0;
    }

    std::string fname = path.substr(lastSlash+1);
    if(fname.size() > MAX_NAME_LENGTH) {
        return 0;
    }

    EntryForFile *oldf = findFile(path, true);
    if(oldf != 0) {
        return oldf;
    }

    EntryForFile *dir = findFile(path.substr(0, lastSlash));
    if(!dir) {
        return 0;
    }
    if(!(dir->attr & FLAG_DIR)) {
        return 0;
    }

    DirIterator dit(this, getBlock(dir->firstBlock), false);  //~
    EntryForFile *fe = dit.current();
    while(dit.hasNext() && fe->attr & FLAG) {
        fe = dit.next();
    }
    if(dit.atBorder()) {
        int inewblock = allocateBlock();
        if(inewblock == -1) {
            return 0;
        }
        dit.currentBlock()->header()->next = inewblock;
        Block *newblock = getBlock(inewblock);
        newblock->header()->next = -1;
        fe = (EntryForFile*)newblock->data();
    } else {
        ++fe;
    }

    fe->attr = FLAG;
    fe->blocks = alloc ? 1 : 0;
    fe->size = 0;
    strncpy(fe->filename, fname.c_str(), MAX_NAME_LENGTH);
    fe->modTime = time(0);
    fe->firstBlock = alloc ? allocateBlock() : -1;

    if(alloc && fe->firstBlock == -1 && flag) {
        return 0;
    }

    dit.dirHeader()->fileCount++;
    return fe;
}

EntryForFile *FileSystem::forcedCreateFile(const std::string &path, bool alloc) {
    size_t lastSlash = path.find_last_of('/');
    if(path[0] != '/' || lastSlash == std::string::npos || lastSlash == path.size()-1 ) {
        return 0;
    }

    EntryForFile *oldf = findFile(path, true);
    if(oldf != 0) return oldf;

    size_t cpos = 1;
    EntryForFile *curDir = &root;
    while(1) {
        size_t spos = path.find_first_of('/', cpos);
        std::string ndir = spos != std::string::npos ? path.substr(cpos, spos-cpos) : path.substr(cpos);
        if(spos == std::string::npos) break;

        if(ndir.size() > MAX_NAME_LENGTH) {
            return 0;
        }

        curDir = findFile(path.substr(0, spos), true);
        if(!curDir) curDir = createDir(path.substr(0, spos));
        cpos = spos+1;
    }

    return createFile(path, alloc);
}

EntryForFile *FileSystem::createDir(const std::string &path) {
    EntryForFile *dir = createFile(path, true);
    if(!dir) return 0;

    dir->attr |= FLAG_DIR;
    HeaderForDirectory *dh = (HeaderForDirectory*)getBlock(dir->firstBlock)->data();
    dh->fileCount = 0;
    EntryForFile *ndir = (EntryForFile*)(getBlock(dir->firstBlock)->data()+sizeof(HeaderForDirectory));
    ndir->copy(dir);
    return dir;
}

EntryForFile *FileSystem::forcedCreateDir(const std::string &path) {
    size_t lastSlash = path.find_last_of('/');
    if(path[0] != '/' || lastSlash == std::string::npos || lastSlash == path.size()-1 ) {
        return 0;
    }

    EntryForFile *oldf = findFile(path, true);
    if(oldf != 0) return oldf;

    size_t cpos = 1;
    EntryForFile *curDir = &root;
    while(1) {
        size_t spos = path.find_first_of('/', cpos);
        std::string ndir = spos != std::string::npos ? path.substr(cpos, spos-cpos) : path.substr(cpos);
        if(spos == std::string::npos) break;

        if(ndir.size() > MAX_NAME_LENGTH) {
            return 0;
        }

        curDir = findFile(path.substr(0, spos), true);
        if(!curDir) curDir = createDir(path.substr(0, spos));
        cpos = spos+1;
    }

    return createDir(path);
}

// ---

bool FileSystem::importFile(const std::string &src, const std::string &dst) {
    FILE *srcFile = fopen(src.c_str(), "rb");
    if(!srcFile) {
        return false;
    }

    fseek(srcFile, 0, SEEK_END);
    size_t sz = ftell(srcFile);

    if(sz > freeSpace()) {
        return false;
    }

    fseek(srcFile, 0, SEEK_SET);
    char *buf = new char[S];


    EntryForFile *destFile = findFile(dst, true);
    if(destFile) {
        removeFile(dst);
    }
    destFile = forcedCreateFile(dst, true);
    if(!destFile) return false;

    Block *curBlock = getBlock(destFile->firstBlock);
    size_t brc = 0;
    size_t blockCount = 1;
    while(true) {
        size_t br = fread(buf, sizeof(char), S-sizeof(HeaderForBlock), srcFile);
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
    EntryForFile *srcFile = findFile(src);
    if(!srcFile || srcFile->attr & FLAG_DIR) return false;

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

// ---

bool FileSystem::removeFile(const std::string &path, bool recursive) {
    EntryForFile *fe = findFile(path);
    if(!fe || (fe->attr & FLAG_DIR)) return false;

    size_t lastSlash = path.find_last_of('/');
    EntryForFile *pdir = findFile(path.substr(0, lastSlash));
    HeaderForDirectory *pdh = (HeaderForDirectory*)getBlock(pdir->firstBlock)->data();

    int cur = fe->firstBlock;
    for(int i=0; i<fe->blocks; i++) {
        int next = getBlock(cur)->header()->next;
        freeBlock(cur);
        if(next == -1) break;
        cur = next;
    }

    fe->attr ^= FLAG;
    if(!recursive) pdh->fileCount--;

    return true;
}

bool FileSystem::remove(const std::string &path, bool recursive) {
    if(path == "/") return false; //need format?
    EntryForFile *fe = findFile(path);
    if(!fe) return false;
    if(!(fe->attr & FLAG_DIR)) return removeFile(path, recursive);

    size_t lastSlash = path.find_last_of('/');
    EntryForFile *pdir = findFile(path.substr(0, lastSlash));
    HeaderForDirectory *pdh = (HeaderForDirectory*)getBlock(pdir->firstBlock)->data();

    DirIterator dit(this, getBlock(fe->firstBlock));
    EntryForFile *cdfe = dit.current();
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

    fe->attr ^= FLAG;
    if(!recursive) pdh->fileCount--;

    return true;
}

// ---

bool FileSystem::move(const std::string &src, const std::string &dst) {

    if( dst.size() >= src.size() && dst.substr(0, src.size()) == src){
        return false;
    }
    if(src == dst) {
        return false;
    }
    EntryForFile *srcFE = findFile(src);
    if(!srcFE) {
        return false;
    }

    size_t lastSlash = src.find_last_of('/');
    //родина исходного файла или директории
    EntryForFile *pdir = findFile(src.substr(0, lastSlash));
    HeaderForDirectory *pdh = (HeaderForDirectory*)getBlock(pdir->firstBlock)->data();

    EntryForFile *dstFE = findFile(dst);

    if( dstFE != 0 && !(dstFE->attr & FLAG_DIR) && (srcFE->attr & FLAG_DIR)) {
        return false;
    }

    // если оба файла
    if( dstFE != 0 && !(dstFE->attr & FLAG_DIR) && !(srcFE->attr & FLAG_DIR)) {

        std::string nameDst = dstFE->name();
        std::string nameSrc = srcFE->name();
        std::string pathDst = "/";
        std::string pathSrc = "/";

        size_t lastSlash = dst.find_last_of('/');
        if(lastSlash != 0) {
            pathDst = dst.substr(0, lastSlash) + "/";
        }
        lastSlash = src.find_last_of('/');
        if(lastSlash != 0) {
            pathSrc = src.substr(0, lastSlash) + "/";
        }

        if( !removeFile(dst) ) {
            return false;
        }

        if( pathDst != pathSrc && !move(src, pathDst) ){
           return false;
        }

        EntryForFile *pdir = findFile(pathDst + nameSrc);
        if(pdir  == 0) {
            return false;
        }

        std::strcpy(pdir->filename, nameDst.c_str());  // аккуратно переименовать
        //pdir->name();

        return true;
    }

    //есть ли родина у dst
    size_t lastSlash2 = dst.find_last_of('/');
    if(dst[0] != '/' || lastSlash2 == std::string::npos ) { //|| lastSlash2 == dst.size()-1
        return 0;
    }
    std::string namePathDst = dst.substr(0, lastSlash2);
    EntryForFile* pdirDst = findFile(namePathDst);
    if(pdirDst == 0 && dst != "/") {
        return false;
    }



    // если нет такого, то создаем
    if(!dstFE) {

        if((srcFE->attr & FLAG_DIR)) {

                dstFE = forcedCreateDir(dst);
                DirIterator dit(this, getBlock(srcFE->firstBlock), true);
                while(dit.hasNext()){
                    std::string name = dit.next()->name();
                    if(! move(src + "/" + name, dst)){
                        return false;
                    }
                    DirIterator bufIt(this, getBlock(srcFE->firstBlock), true);
                    dit = bufIt;
                }
                remove(src);
                return true;

        } else {

            EntryForFile *copyFE = createFile(dst, true);
            if(!copyFE) {
                return false;
            }
            int srcBlock = srcFE->firstBlock;
            int dstBlock = copyFE->firstBlock;

            for(int i=0; i<srcFE->blocks; i++) {
                Block *sblock = getBlock(srcBlock);
                Block *dblock = getBlock(dstBlock);
                memcpy(dblock->data(), sblock->data(), dblock->size());

                if(sblock->header()->next == -1) {
                    dblock->header()->next = -1;
                    break;
                }
                int newblock = allocateBlock();
                if(newblock == -1) {
                    return false;
                }
                dblock->header()->next = newblock;
                srcBlock = sblock->header()->next;
                dstBlock = newblock;
            }

            int fb = copyFE->firstBlock;
            copyFE->copy(srcFE);
            std::strcpy(copyFE->filename, dst.substr(lastSlash2+1).c_str());
            copyFE->firstBlock = fb;
            copyFE->modTime = time(0);

            remove(src);
            return true;
        }
//////////////////////////////////////////////////////////////////
    } else {
        std::string testDst = dst + (dst[dst.size()-1] == '/' ? "" : "/") + src.substr(lastSlash+1);
        EntryForFile *testDstFE = findFile(testDst, true);

        // уже есть такая папка внутри
        if(testDstFE && testDstFE->attr & FLAG_DIR) {
            return false;
        }

        //есть файл с тем же именем внутри, то подменяем
        if(testDstFE && !(testDstFE->attr & FLAG_DIR) && !(srcFE->attr & FLAG_DIR)) {
            return move(src, testDst);
        }
    }


    DirIterator dit(this, getBlock(dstFE->firstBlock), false);
    dstFE = dit.current();
    while(dit.hasNext() && dstFE->attr & FLAG) dstFE = dit.next();
    if(dit.atBorder()) {
        int inewblock = allocateBlock();
        if(inewblock == -1) {
            return false;
        }
        dit.currentBlock()->header()->next = inewblock;
        Block *newblock = getBlock(inewblock);
        newblock->header()->next = -1;
        dstFE = (EntryForFile*)newblock->data();
    } else if(!dit.hasNext()) {
        ++dstFE;
    }
    dstFE->copy(srcFE);
    dit.dirHeader()->fileCount++;
    pdh->fileCount--;
    srcFE->attr ^= FLAG;
    return true;
}

// ---

bool FileSystem::copyFile(EntryForFile *src, const std::string &dst) {

    std::string dst_c = dst;
    if( dst.size() > 1 && dst[0] == '/' && dst[1] == '/') {
        dst_c = dst.substr(1, dst.size());
    }

    size_t lastSlashDst = dst_c.find_last_of('/');
    std::string dstFilename = dst_c.substr(lastSlashDst + 1);

    std::string dstFile;
    if(src->filename == dstFilename) dstFile = dst_c;
    else dstFile = dst_c + "/" + src->name();
    EntryForFile *testFE = findFile(dstFile, true);
    if(testFE) {
        remove(dstFile);
    }

    EntryForFile *copyFE = createFile(dstFile, true);
    if(!copyFE) {
        return false;
    }

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
    EntryForFile *curFE = dit.current();
    std::string dstDirPath = dst + "/" + curFE->name();
    EntryForFile *dstFE = findFile(dstDirPath, true);
    if(!dstFE) {
		dstFE = createDir(dstDirPath);
        if(!dstFE) {
            return false;
        }
    }
    
    while(dit.hasNext()) {
        curFE = dit.next();
        bool res = (curFE->attr & FLAG_DIR) ? copyDir(getBlock(curFE->firstBlock), dstDirPath) : copyFile(curFE, dstDirPath);
        if(!res) {
            return false;
        }
    }
    return true;
}

bool FileSystem::copy(const std::string &src, const std::string &dst_) {

    if( dst_.size() >= src.size() && dst_.substr(0, src.size()) == src){
        return false;
    }

    std::string dst = dst_;
    if( dst.size() > 1 && dst[dst.size()-1] == '/') {
        dst = dst.substr(0, dst.size()-1);
    }

    //~
    size_t lastSlash = src.find_last_of('/');
    std::string ppath = src.substr(0, lastSlash);

    if(ppath.empty()) ppath = "/";
    if(ppath == dst) return true;

    EntryForFile *srcFE = findFile(src);
    if(!srcFE) {
        return false;
    }

    EntryForFile *dstFE = findFile(dst, true);
    if(!dstFE) {
        dstFE = forcedCreateDir(dst);
    } else if (!(dstFE->attr & FLAG_DIR)) {
        return false;
    }

    if(srcFE->attr & FLAG_DIR) {
        size_t epos = std::string::npos;
        while(1) {
            size_t lspos = dst.find_last_of('/', epos);
            if(lspos == std::string::npos) break;
            std::string dpath = dst.substr(0, lspos);
            if(dpath.empty()) break;
            //~
            if(dpath == src) {
                flag = 1;
            }
            epos = lspos-1;
        }
    }

    bool res = (srcFE->attr & FLAG_DIR) ? copyDir(getBlock(srcFE->firstBlock), dst) : copyFile(srcFE, dst);
    return res;
}


bool FileSystem::directoryInfo(EntryForFile *dir) {
    DirIterator dit(this, getBlock(dir->firstBlock));
    std::cout << " contain " << dit.dirHeader()->fileCount << " files:" << std::endl;
    EntryForFile *fe = dit.current();
    while(dit.hasNext()) {
        fe = dit.next();
        std::string sep = strlen(fe->filename) > 8 ? "\t" : "\t\t";
        if(fe->attr & FLAG_DIR) {
            std::cout << "dir:" << sep << fe->name() << std::endl;
        } else {
            std::cout << "file:" << sep << fe->info();
        }
    }
    return true;
}

bool FileSystem::fileInfo(const std::string &path) {
    EntryForFile *f = findFile(path);
    if(!f) return false;
    if(f->attr & FLAG_DIR) {
        std::cout <<"path  " << path <<"  ";
        return directoryInfo(f);
    } else std::cout << f->info();
    return true;
}

// ---

void FileSystem::writeSuperBlock() {
    int curBlockSize = S - sizeof(InfoForSuperBlock);

    std::string blockPath = getBlockPath(0);
    FILE *blockFile = fopen(blockPath.c_str(), "wb");
    fwrite(&sBlock, sizeof(InfoForSuperBlock), 1, blockFile);
    if(sBlockLength == 1) {
        fwrite(sBlock.map, sizeof(char), sBlock.mapSize, blockFile);
        stretchBlock(blockFile);
    } else fwrite(sBlock.map, sizeof(char), curBlockSize, blockFile);
    fclose(blockFile);

    int mapWritten = curBlockSize;
    HeaderForBlock bh;
    for(int i=1; i<sBlockLength; i++) {
        blockFile = fopen(getBlockPath(i).c_str(), "wb");
        bh.next = i == sBlockLength - 1? -1 : i+1;
        fwrite(&bh, sizeof(HeaderForBlock), 1, blockFile);
        if(i != sBlockLength - 1) fwrite(sBlock.map+mapWritten, sizeof(char), S-sizeof(HeaderForBlock), blockFile);
        else {
            fwrite(sBlock.map+mapWritten, sizeof(char), sBlock.mapSize-mapWritten, blockFile);
            stretchBlock(blockFile);
        }
        mapWritten += S-sizeof(HeaderForBlock);
        fclose(blockFile);
    }
}

// ---

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
        if(sBlock.map[p] & (1 << s)) {
            std::cout << '1';
        } else {
            std::cout << '0';
        }
    }
    std::cout << std::endl;
}

void FileSystem::stretchBlock(FILE *block) {
    char a = 0;
    fseek(block, S, SEEK_SET);
    fwrite(&a, sizeof(char), 1, block);
}

// ---

EntryForFile *FileSystem::findFile(const std::string &path, bool test) {

    if(path == "/" || path.empty()) {
        return &root;
    }
    if(path[0] != '/') {
        return 0;
    }
    std::string rpath = path[path.size()-1] == '/' ? path.substr(0, path.size()-1) : path;

    size_t cpos = 1;
    EntryForFile *curDir = &root;
    while(1) {
        size_t spos = rpath.find_first_of('/', cpos);
        std::string ndir = spos != std::string::npos ? rpath.substr(cpos, spos-cpos) : rpath.substr(cpos);
        if(ndir.size() > MAX_NAME_LENGTH) {

            return 0;
        }

        bool found = false;
        DirIterator dit(this, getBlock(curDir->firstBlock));
        EntryForFile *fe = dit.current();
        while(dit.hasNext()) {
            fe = dit.next();
            if(fe->name() == ndir) {
                if(spos == std::string::npos) return fe;
                else if(! (fe->attr & FLAG_DIR) ) {
                    return 0;

                }
                found = true;
                curDir = fe;
                break;
            }
        }

        if(!found) {
            return 0;
        }

        cpos = spos+1;
    }
    return 0;
}

/////////////

DirIterator::DirIterator(FileSystem *fsys, Block *b, bool skipEmpty) {
    curBlock = b;
    dh = (HeaderForDirectory*)b->data();
    curFE = (EntryForFile*)(b->data()+sizeof(HeaderForDirectory));
    pos = 0;
    skip = skipEmpty;
    fs = fsys;
}

bool DirIterator::hasNext() const {
    return pos < dh->fileCount;
}

EntryForFile *DirIterator::current() {
    return curFE;
}

EntryForFile *DirIterator::next() {
    while(1) {
        ++curFE;
        if((char*)curFE+sizeof(EntryForFile) > curBlock->data()+curBlock->size()) {
            curBlock = fs->getBlock(curBlock->header()->next);
            curFE = (EntryForFile*)curBlock->data();
        }
        if(curFE->attr & FLAG) {
            pos++;
            break;
        } else if(!skip) break;
    }
    return curFE;
}

bool DirIterator::atBorder() const {
    return ((char*)curFE+2*sizeof(EntryForFile) > curBlock->data()+curBlock->size());
}

Block *DirIterator::currentBlock() {
    return curBlock;
}

HeaderForDirectory *DirIterator::dirHeader() {
    return dh;
}

