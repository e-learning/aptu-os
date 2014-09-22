#include "general.h"

PipeRedirect parse_command(int argc, char** argv, char** cmd1, char** cmd2) {
  PipeRedirect result = NEITHER;

  int split = -1;

  for (int i=0; i<argc; i++) {
    if (strcmp(argv[i], "|") == 0) {
      result = PIPE;
      split = i;

    } else if (strcmp(argv[i], ">>") == 0) {
      result = REDIRECT;
      split = i;
    }
  }

  if (result != NEITHER) {
    for (int i=0; i<split; i++)
      cmd1[i] = argv[i];

    int count = 0;
    for (int i=split+1; i<argc; i++) {
      cmd2[count] = argv[i];
      count++;
    }

    cmd1[split] = NULL;
    cmd2[count] = NULL;
  }

  return result;
}

void pipe_cmd(char** cmd1, char** cmd2) {
  int fds[2];
  pipe(fds);
  pid_t pid;

  if (fork() == 0) {
    dup2(fds[0], 0);

    close(fds[1]);

    execvp(cmd2[0], cmd2);
    perror("execvp failed");

  } else if ((pid = fork()) == 0) {
    dup2(fds[1], 1);

    close(fds[0]);

    execvp(cmd1[0], cmd1);
    perror("execvp failed");

  } else
    waitpid(pid, NULL, 0);
}

int read_args(char **argv) {
  char *cstr;
  string arg;
  int argc = 0;

  while (cin >> arg) {
    if (want_to_quit(arg)) {
      cout << "Goodbye!\n";
      exit(0);
    }

    cstr = new char[arg.size()+1];
    strcpy(cstr, arg.c_str());
    argv[argc] = cstr;

    argc++;

    if (cin.get() == '\n')
      break;
  }

  argv[argc] = NULL;

  return argc;
}

void redirect_cmd(char** cmd, char** file) {
  int fds[2];
  int count;
  int fd;
  char c;
  pid_t pid;

  pipe(fds);

  if (fork() == 0) {
    fd = open(file[0], O_RDWR | O_CREAT, 0666);

    if (fd < 0) {
      printf("Error: %s\n", strerror(errno));
      return;
    }

    dup2(fds[0], 0);

    close(fds[1]);

    while ((count = read(0, &c, 1)) > 0)
      write(fd, &c, 1);

    execlp("echo", "echo", NULL);

  } else if ((pid = fork()) == 0) {
    dup2(fds[1], 1);

    close(fds[0]);

    execvp(cmd[0], cmd);
    perror("execvp failed");

  } else {
    waitpid(pid, NULL, 0);
    close(fds[0]);
    close(fds[1]);
  }
}

void run_cmd(int argc, char** argv) {
  pid_t pid;
  const char *amp;
  amp = "&";
  bool found_amp = false;

  if (strcmp(argv[argc-1], amp) == 0)
    found_amp = true;

  pid = fork();

  if (pid < 0)
    perror("Error (pid < 0)");

  else if (pid == 0) {
    if (found_amp) {
      argv[argc-1] = NULL;
      argc--;
    }

    execvp(argv[0], argv);
    perror("execvp error");

  } else if (!found_amp)
    waitpid(pid, NULL, 0);
}

bool want_to_quit(string choice) {
  for (unsigned int i=0; i<choice.length(); i++)
    choice[i] = tolower(choice[i]);

  return (choice == "quit" || choice == "exit");
}
