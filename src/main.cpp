#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAXLINE 8192
#define MAXARGS 128
void eval(char *cmdline);
int parseline(char *buf, char *argv[]);
int builtin_command(char *argv[]);
int main(int argc, char *argv[], char *envp[]) {
  char cmdline[MAXLINE];
  while (true) {
    printf("%s ", getenv("PWD"));
    printf("> ");
    fgets(cmdline, MAXLINE, stdin);
    if (feof(stdin))
      exit(0);
    eval(cmdline);
  }
}

// evaluate
void eval(char *cmdline) {
  char *argv[MAXARGS]; // argument list execve()
  char buf[MAXLINE];   // command line
  int bg;              // background
  pid_t pid;

  strcpy(buf, cmdline);
  bg = parseline(buf, argv);
  if (argv[0] == NULL)
    return;

  if (!builtin_command(argv)) {
    if ((pid = fork()) == 0) { // child
      if (execve(argv[0], argv, environ) < 0) {
        printf("%s: Command not found.\n", argv[0]);
        exit(0);
      }
    }

    if (!bg) {
      int status;
      waitpid(pid, &status, 0);
    } else {
      printf("%d %s", pid, cmdline);
    }
  }
}
// if first arg is a buitin command,run and return true
int builtin_command(char *argv[]) {
  if (!strcmp(argv[0], "exit"))
    exit(0);

  if (!strcmp(argv[0], "echo")) {
    for (int i = 1; argv[i] != NULL; i++)
      printf("%s ", argv[i]);
    printf("\n");
    return 1;
  }

  if (!strcmp(argv[0], "pwd")) {
    printf("%s\n", getenv("PWD"));
    return 1;
  }

  if (!strcmp(argv[0], "cd")) {

    // how?

    return 1;
  }

  if (!strcmp(argv[0], "&"))
    return 1;
  return 0;
}
// pause command line and build the argv array
int parseline(char *buf, char *argv[]) {
  char *delim;
  int argc;
  int bg;
  buf[strlen(buf) - 1] = ' ';

  while (*buf && (*buf == ' '))
    buf++;

  argc = 0;
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

  if ((bg = (*argv[argc - 1] == '&')) != 0)
    argv[--argc] = NULL;

  return bg;
}