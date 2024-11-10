// FILE: xargs.cpp
#include "../include/command_line.hpp"
#include <cstdio>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;
int main(int argc, char *argv[]) {

  argc = 0;
  for (int i = 0; argv[i] != NULL; i++)
    ++argc;

  char buffer[8192];
  ssize_t bytesRead;
  while ((bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {
    buffer[bytesRead] = '\0';

    parseline(buffer, argv, argc);
    argc = 0;
    for (int i = 0; argv[i] != NULL; i++)
      ++argc;
  }

  // 执行命令

  pid_t pid = fork();
  if (pid == 0) {

    if (!builtin_command(argv + 1)) {
      // for (int i = 0; argv[i] != NULL; i++)
      //   fprintf(stderr, "%s@qwq ", argv[i]);
      execve_command(argv + 1);
    }

  } else if (pid > 0) {
    waitpid(pid, NULL, 0);
  }
  exit(0);
}