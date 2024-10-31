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

  for (int i = 0; envp[i] != NULL; i++)
    printf("envp[%d] : %s\n", i, envp[i]);

  char cmdline[MAXLINE];

  while (true) {
    printf("%s@%s ", getenv("USER"), getenv("LOGNAME"));
    char buf[MAXLINE];
    strcpy(buf, getenv("PWD"));

    char *tmp = strrchr(buf, '/');
    if (strcmp(buf, "/"))
      strcpy(buf, tmp + 1);
    printf("%s > ", buf);

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

    if (argv[1][strlen(argv[1]) - 1] == '/') {
      if (strlen(argv[1]) == 1)
        setenv("PWD", "/", 1);
      else
        argv[1][strlen(argv[1]) - 1] = '\0';
    }

    if (!strcmp(argv[1], "."))
      ;
    // chdir(getenv("PWD"));

    else if (!strcmp(argv[1], "..")) {
      char buf[MAXLINE];
      strcpy(buf, getenv("PWD"));
      char *tmp = strrchr(buf, '/');

      if (&buf[0] == tmp)
        tmp++; // '/home' -> '/'
      *tmp = '\0';

      if (!chdir(buf))
        setenv("PWD", buf, 1);
      else
        printf("%s is not a valid directory.\n", buf);
      // printf("%s\n", buf);
      // printf("ok = %d\n", chdir(buf));

    }

    else if (argv[1][0] != '/') {
      char buf[MAXLINE];
      strcpy(buf, getenv("PWD"));
      char *tmp = buf;
      strcat(buf, "/");
      strcat(buf, argv[1]);
      if (!chdir(buf))
        setenv("PWD", buf, 1);
      else
        printf("%s is not a valid directory.\n", buf);

    }

    else {
      if (!chdir(argv[1]))
        setenv("PWD", argv[1], 1);
      else
        printf("%s is not a valid directory.\n", argv[1]);
      // chdir(argv[1]);
    }

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