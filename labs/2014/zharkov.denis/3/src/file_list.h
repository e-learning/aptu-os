#ifndef FILE_LIST_H
#define FILE_LIST_H

#include <iterator>
#include <algorithm>
#include <ctime>
#include "myfs.h"
#include "list.h"


struct FileListBlock : ListOnBlocksBlock  {
    static const size_t block_id_offset = 0;
    static const size_t name_offset     = block_id_offset + sizeof(block_id_t);
    static const size_t is_dir_flag_offset = name_offset + max_filename_length;
    static const size_t file_size_offset = is_dir_flag_offset + 1;
    static const size_t timestamp_offset = file_size_offset + sizeof(size_t);
    
    static constexpr const size_t slot_size = timestamp_offset + sizeof(time_t);
    
    struct FileInFileList {
        size_t slot;
        File file;
    };

    FileListBlock(BlockPtr block) 
        : ListOnBlocksBlock(move(block), slot_size) {
        load();
    }

    void load() {
        ListOnBlocksBlock::load();

        size_t offset_from_begin = header_size;
        files_.clear();
        free_slots_.clear();

        for (size_t slot = 0; slot < slots_count(); ++slot) {
            size_t offset = offset_from_begin + slot_size * slot;

            block_id_t id = block_->get_int<block_id_t>(offset + block_id_offset);
            
            if (id == 0) {
                free_slots_.push_back(slot);
                continue;
            }

            File file;
            file.block_id = id;

            file.name = block_->get_string(offset + name_offset, max_filename_length);
            file.is_dir = block_->byte(offset + is_dir_flag_offset);
            file.size = block_->get_int<size_t>(offset + file_size_offset);
            file.last_modification_time = block_->get_int<time_t>(offset + timestamp_offset); 
            
            files_.push_back({slot, file});
        }
    }

    void save_file_info(File const & file, size_t slot) {
        size_t offset = slot_offset(slot);

        block_->put_int(offset + block_id_offset, file.block_id);
        block_->put_string(offset + name_offset, file.name, max_filename_length);
        block_->byte(offset + is_dir_flag_offset) = file.is_dir;

        block_->put_int(offset + file_size_offset, file.size);
        block_->put_int(offset + timestamp_offset, file.last_modification_time); 
    }

    void insert_file_info(File const & file) {
        save_file_info(file, free_slots_.back());
        free_slots_.pop_back();
    }

    vector<size_t> free_slots_;
    vector<FileInFileList> files_;
};

typedef shared_ptr<FileListBlock> FileListBlockPtr;

struct FileList {
    FileList(File file)
        : first_block_(file.block_id)    
    {

    }

    FileList(block_id_t first_block)
         : first_block_(first_block)
    {

    }

    struct Iter : iterator<forward_iterator_tag, File> {
        explicit Iter() {
            current_block_ = nullptr;
        }

        explicit Iter(block_id_t first_block_id) 
        {
            current_block_ = load_block(first_block_id);
            file_in_block_ = current_block_->files_.begin();            
            move_until_next(); 
        }

        FileListBlockPtr current_block_;
        vector<FileListBlock::FileInFileList>::iterator file_in_block_;

        Iter & operator++() {
            ++file_in_block_;
            move_until_next(); 
            
            return *this;
        }

        void move_until_next() {
            if ((file_in_block_) == current_block_->files_.end()) {
                size_t next_block_id = current_block_->next_block(); 
                
                if (next_block_id == 0) {
                    current_block_ = nullptr;
                } else {
                    current_block_ = load_block(next_block_id);
                    file_in_block_ = current_block_->files_.begin();
                }
            }
        }

        File & operator*() {
            return file_in_block_->file;
        }
        
        File * operator->() {
            return &(file_in_block_->file);
        }

        bool operator==(const Iter & other) {
            if (current_block_ == nullptr || other.current_block_ == nullptr) {
                return current_block_ == other.current_block_;
            }

            return current_block_ == other.current_block_ && file_in_block_ == other.file_in_block_;
        }

        bool operator!=(const Iter & other) {
            return ! (*this == other);
        }
    };

    typedef File value_type;
    typedef Iter iterator;
    
    Iter begin() {
        return Iter(first_block_);
    }

    Iter end() {
        return Iter();
    }

    Iter find(const string & name) {
        return find_if(begin(), end(), [&name](const File & o) { return o.name == name; });
    }

    bool exists(const string & name) {
        return find(name) != end();
    }

    void init() {
        load_block(first_block_)->block_->clear_data();
    }

    void insert(File file) {
        file.last_modification_time = time(NULL);
        MyFileSystem & fs = MyFileSystem::instance();
        
        auto first_block = load_block(first_block_);
        
        if (first_block->free_slots_.empty()) {
            block_id_t free_block_id = fs.acquire_free_block();
            BlockPtr new_block = fs.load_block(free_block_id);

            new_block->clear_data();

            first_block->block_->swap_data(*new_block);
            fs.write_block(*new_block);
            
            first_block->load();
            first_block->set_next(free_block_id);
        }
        
        first_block->insert_file_info(file);
        write_block(first_block);
    }
    
    void remove(block_id_t block_id, size_t slot) {
        MyFileSystem & fs = MyFileSystem::instance();
        
        auto first_block = load_block(first_block_);
        FileListBlock::FileInFileList file_from_first_block = first_block->files_.back();
        
        first_block->files_.pop_back();
        
        if (block_id != first_block_) {
            auto block = load_block(block_id);
            
            block->save_file_info(file_from_first_block.file, slot);
            write_block(block);
        } else {
            if (file_from_first_block.slot != slot) {
                first_block->save_file_info(file_from_first_block.file, slot);
            } 
        }
        
        first_block->free_slot(file_from_first_block.slot);
        
        if (first_block->files_.empty() && first_block->next_block() != 0) {
            block_id_t second_block_id = first_block->next_block();
            auto second_block = load_block(second_block_id);
            first_block->block_->swap_data(*(second_block->block_));
            
            fs.release_block(second_block_id);
        }   

        write_block(first_block);
    }

    void remove(Iter & iter) {
        remove(iter.current_block_->block_->id_, iter.file_in_block_->slot);   
    }
    
    void clear() {
        auto block = load_block(first_block_);
        
        while (block->next_block() != 0) {
            MyFileSystem::instance().release_block(block->block_->id_);
            block = load_block(block->next_block());
        }
        
        MyFileSystem::instance().release_block(block->block_->id_);
    }

    static FileListBlockPtr load_block(block_id_t id) {
        return FileListBlockPtr(new FileListBlock(MyFileSystem::instance().load_block(id))); 
    }
    static void write_block(const FileListBlockPtr & block) {
        MyFileSystem::instance().write_block(*(block->block_));
    }
           
    block_id_t first_block_;
};
#endif
