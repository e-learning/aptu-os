#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<dirent.h>

#include<fstream>
#include<string.h>

#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>

#include <signal.h>



bool is_digit( std::string& str)
{
  for ( std::string::iterator it=str.begin(); it!=str.end(); ++it)
    if( !isdigit(*it) ) return false;
  return true;
}


void start_ls(std::vector<std::string> & args)
{
   if( !args.empty() ) 
   {
      std::cout << "this command have not parameters" << "\n";
      return;   
   }
   DIR *dir;
   dirent *ent;
   if ((dir = opendir (".")) != NULL) 
   {
     while ((ent = readdir (dir)) != NULL) 
     {
       std::cout << ent->d_name << "\n";
     }
     closedir (dir);
   } 
   else 
   {
     std::cout << "could not open directory";
   }
}


void start_pwd(std::vector<std::string> & args)   
{
   if( !args.empty() ){
      std::cout << "this command have not parameters" << "\n";
      return;   
   }
   std::cout << get_current_dir_name() << "\n";
}


void start_ps(std::vector<std::string> & args)
{
    if( !args.empty() )
    {
      std::cout << "this command have not parameters" << "\n";
      return;   
    }
    
    DIR *dir;
    dirent *ent;
    std::ifstream in;
    
    if( dir = opendir("/proc") )
    {
        while( ent = readdir(dir) )
        {
            std::string num_proc(ent->d_name);
            if( is_digit(num_proc) )
            {
                std::string path_proc("/proc/" + num_proc + "/comm");
                in.open( path_proc.c_str() );
                std::string name_proc;
                in >> name_proc;
                in.close();
                std::cout << num_proc << "\t" << name_proc << "\n";
            }
        }
        closedir(dir);
    }
}

void start_kill( std::vector<std::string> & args )
{
    
    if ( args.size() != 2 || !is_digit(args[0]) || !is_digit(args[1]) ) 
    {
        std::cout << "this command have two digital parameters" << "\n";
    }
    else
    {
      pid_t pid = atoi(args[1].c_str());
      int sig = atoi(args[0].c_str());
      if ( kill(pid,sig) == -1 )
      std::cout << "error. First parameter - signal, second parameter - process" << "\n";
    }
}


int main()
{

   while(1)
   {
      std::string str;
      std::string command;
      std::vector<std::string> args;

      std::cout << ">> ";
      std::getline(std::cin, str);
      
      std::istringstream ist(str);
      std::string tmp;
      
      if ( ist >> tmp )
      {
         command = tmp;
         if(command == "exit") break;    
      }   
      while ( ist >> tmp )
      {
         args.push_back(tmp);
      }  
      

      if( command == "ls")  start_ls(args);
      else if( command == "ps") start_ps(args);
      else if( command == "pwd") start_pwd(args);
      else if( command == "kill") start_kill(args);
      else system(command.c_str());
      
   }     
       
   return 0;
}











