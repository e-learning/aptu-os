#include <stdio.h>
#include "allocator.h"
#include <iostream>
using namespace std;

int parse_command(string input_str, string &command, string &arg)
{
    // пропускаем возможные пробелы перед командой
    size_t comm_begin = input_str.find_first_not_of(' ', 0);
    if (comm_begin == std::string::npos)
    {
        return ALLOCATOR_ERROR;
    }
    
    //ищем конец команды
    size_t comm_end = input_str.find_first_of(" ", comm_begin);
    if (comm_end == std::string::npos)
    {
        comm_end = input_str.length();
    }
    
    // извлекаем команду из строки
    command = input_str.substr(comm_begin, comm_end - comm_begin);
   
    //ищем начало аргумента команды
    size_t arg_begin = input_str.find_first_not_of(' ', comm_end);
    if (arg_begin == std::string::npos) // нету аргументов
    {
        return ALLOCATOR_OK;
    }
    
    //до конца строки - аргумент
    size_t arg_end = input_str.length();
    
    arg = input_str.substr(arg_begin, arg_end - arg_begin);
    
    return ALLOCATOR_OK;
}

void print_help()
{
    cout << "************ АЛЛОКАТОР ПАЯМТИ ************" << endl;
    cout << "Поддерживаемые команды:" <<endl;
    cout << "   help - выводит данное собщение." << endl;
    cout << endl;
    
    cout << "   exit - выход." << endl;   
    cout << endl;
    
    cout << "   alloc s - команда аллокации блока в s байт." << endl; 
    cout << endl;
    
    cout << "   free p - команда освобождения блока выделенного в команде alloc. " << endl; 
    cout << "   p - адрес возвращенный командой alloc." << endl;
    cout << endl;
    
    cout << "   info - команда вывода статистики использования памяти." << endl; 
    cout << endl;
    
    cout << "   map - команда вывода карты памяти." << endl;
    cout << endl;
}

int main(int argc, char** argv)
{
    string input_str, command, arg;
    size_t n;
    
    cout << "Введите размер распределяемого блока памяти в байтах:" << endl;
    getline(cin, input_str);
    
    n = atoi(input_str.c_str());
    
    alloc_init(n);
    
    while(1)
    {
        cout << ">";
        getline(cin, input_str);
        if(parse_command(input_str, command, arg) == ALLOCATOR_OK)
        {
            if(command.compare("alloc") == 0)
            {
                mem_offset_t offset;
                if((offset = alloc_allocate(atoi(arg.c_str()))) >= 0)
                    cout << "+" << offset << endl;
                else
                    cout << "-" << endl;
                continue;
            }
            
            if(command.compare("free") == 0)
            {
                if(alloc_free(atoi(arg.c_str())) == ALLOCATOR_OK)
                    cout << "+" << endl;
                else
                    cout << "-" << endl;
                continue;
            }
            
            if(command.compare("info") == 0)
            {
                alloc_info();
                continue;
            }
            
            if(command.compare("map") == 0)
            {
                alloc_map();
                continue;
            }
            
            if(command.compare("help") == 0)
            {
                print_help();
                continue;
            }
            
            if(command.compare("exit") == 0)
            {
                break;
            }
        }
    }
    alloc_finit();
    return 0;
}
