#include "general.h"

int main() {
  char *argv[MAX_ARGS], *cmd1[MAX_ARGS], *cmd2[MAX_ARGS];
  PipeRedirect pipe_redirect;
  int argc;

  while (true) {
    cout << "> ";

    argc = read_args(argv);

    pipe_redirect = parse_command(argc, argv, cmd1, cmd2);

    if (pipe_redirect == PIPE)
      pipe_cmd(cmd1, cmd2);
    else if (pipe_redirect == REDIRECT)
      redirect_cmd(cmd1, cmd2);
    else
      run_cmd(argc, argv);

    for (int i=0; i<argc; i++)
      argv[i] = NULL;
  }

  return 0;
}
