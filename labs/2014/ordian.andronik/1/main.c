#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


int ls(int argc, char ** argv)
{
  if (argc) 
    {
      fprintf(stderr, "Error: ls flags aren't supported yet...\n");
      return -42;   
    }
  
  DIR * dp;
  struct dirent * entry;
  
  if (dp = opendir(".")) 
    {
      while (entry = readdir(dp)) 
	puts(entry->d_name);
      closedir(dp);
    } 
  else 
    {
      perror("opendir");
      return -1;
    }
   
  return 0;
}


int pwd(int argc, char ** argv)   
{
  if (argc)
    {
      fprintf(stderr, "Error: pwd flags aren't supported yet...");
      return -42;   
    }
  
  char * name = get_current_dir_name();
  printf("%s\n", name);
  free(name);
  
  return 0;
}


int ps(int argc, char ** argv)
{
  if (argc)
    {
      fprintf(stderr, "Error: ps flags aren't supported yet...");
      return -42;   
    }
    
  DIR * dp;
  struct dirent * entry;
  
  if (dp = opendir("/proc"))
    {
      while (entry = readdir(dp))
        {
	  char const * pid = entry->d_name;
	  
	  if (atoi(pid))
            {
	      char path[32];
	      strcpy(path, "/proc/");
	      strcat(path, pid);
	      strcat(path, "/comm");
	      
	      char proc_name[80];
	      FILE * f = fopen(path, "r");
	      fgets(proc_name, 80, f);
	      fclose(f);
	      
	      printf("%s:  %s", pid, proc_name);
            }
	}
      closedir(dp);
    }
  else
    {
      perror("opendir");
      return -1;
    }
  
  return 0;
}

int megakill_usage()
{
  static char const * usage = "Usage: kill -<signal> <pid>";
  fprintf(stderr, "Error: %s\n", usage);
  return -1;
}

int megakill(int argc, char ** argv)
{
  if (argc != 2 ||
      argv[0][0] != '-' ||
      !atoi(argv[0] + 1) ||
      !atoi(argv[1]))
    return megakill_usage();
  else
    {
      int   sig = atoi(argv[0] + 1);
      pid_t pid = atoi(argv[1]);
            
      if (-1 == kill(pid, sig))
	return megakill_usage();
    }
  
  return 0;
}

int run_shell()
{
  while (1)
    {
      char    line[2014];
      char  backup[2014];
      char    cmd[80];
      int     argc = 0;
      char *  argv[64];
      
      printf("➜ ");
      fgets(line, 2014, stdin);
      strcpy(backup, line);
      
      char * str = 0;
      
      if (str = strtok(line, " \n"))
	{
	  strcpy(cmd, str);
	  if (!strcmp(cmd, "exit"))
	    break;
	  str = strtok(NULL, " ");
	}
      
      while (str)
	{
	  argv[argc++] = malloc(80); 
	  strcpy(argv[argc - 1], str);
	  str = strtok(NULL, " ");
	}  

      if      (!strcmp(cmd, "ls"))   ls(argc, argv);
      else if (!strcmp(cmd, "ps"))   ps(argc, argv);
      else if (!strcmp(cmd, "pwd"))  pwd(argc, argv);
      else if (!strcmp(cmd, "kill")) megakill(argc, argv);
      else                           system(backup);

      int i = argc - 1;
      for (; i >= 0; --i)
	free(argv[i]);
    }
  
  return 0;
}

int main(void)
{
  return run_shell();    
}
