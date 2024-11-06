#include <csetjmp>
#include <cstddef>
#include <cstdio>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 8192
#define MAXARGS 128
int eval(char *cmdline);
int parseline(char *buf, char *argv[]);
int builtin_command(char *argv[]);

sigjmp_buf env;
void sigint_handler(int sig) { siglongjmp(env, 1); }

int main(int argc, char *argv[], char *envp[]) {

  signal(SIGINT, sigint_handler);

  while (true) {

    if (sigsetjmp(env, 1) == 1) {
      printf("\n");
      continue;
    }

    char buf[MAXLINE];
    strcpy(buf, getenv("USER"));
    strcat(buf, "@");
    strcat(buf, getenv("LOGNAME"));
    strcat(buf, " ");
    char *tmp = strrchr(getenv("PWD"), '/');
    if (strcmp(getenv("PWD"), "/"))
      strcat(buf, tmp + 1);
    else
      strcat(buf, tmp);
    strcat(buf, " > ");

    char *cmdline = readline(buf);

    if (cmdline == nullptr)
      exit(0);

    if (eval(cmdline)) {
      free(cmdline);
      exit(0);
    }
    free(cmdline);
  }
}

// evaluate
int eval(char *cmdline) {
  char *argv[MAXARGS]; // argument list execve()
  char buf[MAXLINE];   // command line
  pid_t pid;

  strcpy(buf, cmdline);
  parseline(buf, argv);
  if (argv[0] == NULL)
    return 0;

  int exit_flag;
  if (!(exit_flag = builtin_command(argv))) {
    if ((pid = fork()) == 0) { // child
      char ex_path[MAXLINE];
      strcpy(ex_path, "/home/");
      strcat(ex_path, getenv("USER"));
      strcat(ex_path, "/codes/c++/lmksh/command/mine/");
      strcat(ex_path, argv[0]);

      // for (int i = 0; argv[i] != NULL; i++)
      //   printf("argv[%d] : %s\n", i, argv[i]);
      // for (int i = 1; i <= tmpi; i++)
      //   argv[i - 1] = argv[i];

      if (execve(ex_path, argv, environ) < 0) {
        printf("%s: Command not found.\n", argv[0]);
        exit(0);
      }
    }

    int status;
    waitpid(pid, &status, 0);

    return 0;
  }
  if (exit_flag == 2)
    return 1;
  return 0;
}
// if first arg is a buitin command,run and return true
int builtin_command(char *argv[]) {
  if (!strcmp(argv[0], "exit"))
    return 2;

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
    return 1;
  }

  return 0;
}
// pause command line and build the argv array
int parseline(char *buf, char *argv[]) {

  char *delim;
  int argc;
  int bg;
  strcat(buf, " ");

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

  return 0;
}