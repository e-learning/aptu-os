#ifndef DATA_H
#define DATA_H

#include <ctime>
#include <iostream>
#include <vector>
#include <cstring>

struct file_record 
{
    int id;
    bool directory;             
		int first_child;
    int parent_file;            
    int next_file;              
    int prev_file;
    int first_block;            
    size_t number_of_blocks;    
    char name[10];
    time_t time;                
};

struct block_record
{
    int id;                     
    int next;                   
    size_t len;                 
};

struct Config 
{
    int block_size;             
    int block_number;           
};

template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& v) 
{
    os << v.size() << " files" << std::endl;
    for (auto it = v.begin(); it != v.end(); ++it)
        os << *it;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, file_record const& fr) {
    os << (fr.directory ? 'd' : 'f') << "\t '" << fr.name << "'\t";

    if  (fr.directory) 
        os << std::endl;
		else 
		{
        os << fr.number_of_blocks << " blocks";
        os << '\t' << ctime(&fr.time);
    }
    return os;
}

#endif
