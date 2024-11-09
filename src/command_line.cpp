#include "../include/command_line.h"
#include "../include/redirect.h"
#include <stdio.h>

#define MAXLINE 8192
#define MAXARGS 256
// if first arg is a buitin command,run and return true
int builtin_command(char *argv[]) {
  int in = dup(STDIN_FILENO), out = dup(STDOUT_FILENO);
  for (int i = 0; argv[i] != NULL; i++) {
    if (!strcmp(argv[i], "<")) {
      redirect_stdin(argv[++i]);
      continue;
    }
    if (!strcmp(argv[i], ">")) {
      redirect_stdout(argv[++i]);
      continue;
    }
  }

  if (!strcmp(argv[0], "exit"))
    return return_with_reset_inout(in, out, 2);

  if (!strcmp(argv[0], "echo")) {
    for (int i = 1; argv[i] != NULL; i++) {
      if (!strcmp(argv[i], "<") || !strcmp(argv[i], ">")) {
        i++;
        continue;
      }
      printf("%s ", argv[i]);
    }
    printf("\n");
    return return_with_reset_inout(in, out, 1);
  }

  if (!strcmp(argv[0], "pwd")) {
    printf("%s\n", getenv("PWD"));
    return return_with_reset_inout(in, out, 1);
  }

  if (!strcmp(argv[0], "cd")) {

    if (argv[1] == NULL) {
      chdir(getenv("HOME"));
      char cwd[MAXLINE];
      getcwd(cwd, sizeof(cwd));
      setenv("PWD", cwd, 1);
    } else if (chdir(argv[1]) == 0) {
      char cwd[MAXLINE];
      getcwd(cwd, sizeof(cwd));
      setenv("PWD", cwd, 1);
    } else {
      printf("%s is not a valid directory.\n", argv[1]);
    }
    return return_with_reset_inout(in, out, 1);
  }

  // if (!strcmp(argv[0], "xargs")) {
  //   int argc = 0;
  //   for (int i = 0; argv[i] != NULL; i++)
  //     ++argc;

  //   char buffer[8192];
  //   ssize_t bytesRead;
  //   while ((bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0)
  //   {
  //     buffer[bytesRead] = '\0';

  //     parseline(buffer, argv, argc);
  //     argc = 0;
  //     for (int i = 0; argv[i] != NULL; i++)
  //       ++argc;
  //   }

  //   if (!builtin_command(argv + 1)) {
  //     execve_command(argv + 1);
  //   }

  //   return return_with_reset_inout(in, out, 1);
  // }

  return return_with_reset_inout(in, out, 0);
}
// pause command line and build the argv array
int parseline(char *buf, char *argv[], int argc) {

  char *delim;
  strcat(buf, " ");

  while (*buf && (*buf == ' '))
    buf++;

  while ((delim = strchr(buf, ' '))) {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' '))
      buf++;
  }
  argv[argc] = NULL;

  if (argc == 0)
    return 1;

  return 0;
}
void execve_command(char **argv) {
  char ex_path[MAXLINE];
  strcpy(ex_path, getenv("HOME"));
  strcat(ex_path, "/codes/c++/lmksh/command/mine/");
  strcat(ex_path, argv[0]);

  for (int i = 0; argv[i] != NULL; i++) {
    if (!strcmp(argv[i], "<")) {
      redirect_stdin(argv[++i]);
      continue;
    }
    if (!strcmp(argv[i], ">")) {
      redirect_stdout(argv[++i]);
      continue;
    }
  }

  if (execve(ex_path, argv, environ) < 0) {
    printf("%s: Command not found.\n", argv[0]);
    exit(0);
  }
}
