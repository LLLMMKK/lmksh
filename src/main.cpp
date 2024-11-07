#include <csetjmp>
#include <cstddef>
#include <cstdio>
#include <fcntl.h>
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
extern char **environ;
int eval(char *cmdline);
int parseline(char *buf, char *argv[]);
int builtin_command(char *argv[]);

sigjmp_buf env;
void sigint_handler(int sig) { siglongjmp(env, 1); }

int main(int argc, char *argv[]) {

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
    strcat(buf, " $ ");

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

void redirect_stdin(char *argv) {
  int fp = open(argv, O_RDONLY);
  dup2(fp, 0);
  close(fp);
}
void redirect_stdout(char *argv) {
  int fp = open(argv, O_WRONLY | O_CREAT, 0644);
  dup2(fp, 1);
  close(fp);
}
int return_with_reset_inout(int in, int out, int x) {
  dup2(in, 0);
  dup2(out, 1);
  close(in);
  close(out);
  return x;
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

  int argc = 0;
  for (int i = 0; argv[i] != NULL; i++)
    ++argc;

  // i=0 i<argc

  char *tmp_argv[MAXARGS][MAXARGS];
  int command_count = 0, last = 0;
  int command_len[MAXARGS];
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "|")) {

      ++command_count;
      command_len[command_count] = -1;
      for (int j = last; j < i; j++)
        tmp_argv[command_count][++command_len[command_count]] = argv[j];
      last = i + 1;
    }
  }
  ++command_count;
  command_len[command_count] = -1;
  for (int j = last; j < argc; j++)
    tmp_argv[command_count][++command_len[command_count]] = argv[j];

  if (command_count > 1) {

    pid_t Pid;
    if ((Pid = fork()) == 0) {
      pid_t pids[MAXLINE];

      int fds[MAXLINE][2];
      for (int T = 1; T < command_count; T++)
        pipe(fds[T]);

      int child = 0;

      for (int T = 1; T <= command_count; T++) {
        if ((pids[T] = fork()) == 0) {

          if (T < command_count) {
            dup2(fds[T][1], 1);
            close(fds[T][0]);
          }
          if (T > 1) {
            dup2(fds[T - 1][0], 0);
            close(fds[T - 1][1]);
          }
          ++child;
          break;
        }
      }

      if (!child) {
        for (int T = 1; T < command_count; T++)
          close(fds[T][0]), close(fds[T][1]);
      }

      if (child) {
        char **t = tmp_argv[child];
        if (!builtin_command(t)) {
          char ex_path[MAXLINE];
          strcpy(ex_path, getenv("HOME"));
          strcat(ex_path, "/codes/c++/lmksh/command/mine/");
          strcat(ex_path, t[0]);

          for (int i = 0; t[i] != NULL; i++) {
            if (!strcmp(t[i], "<")) {
              redirect_stdin(t[++i]);
              continue;
            }
            if (!strcmp(t[i], ">")) {
              redirect_stdout(t[++i]);
              continue;
            }
          }

          if (execve(ex_path, t, environ) < 0) {
            printf("%s: Command not found.\n", t[0]);
            exit(0);
          }
        }

        exit(0);
      }
    }
    waitpid(Pid, NULL, 0);
    return 0;

  } else {
    int exit_flag;
    if (!(exit_flag = builtin_command(argv))) {
      if ((pid = fork()) == 0) { // child
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

      int status;
      waitpid(pid, &status, 0);

      return 0;
    }

    if (exit_flag == 2)
      return 1;
    return 0;
  }
}

// if first arg is a buitin command,run and return true
int builtin_command(char *argv[]) {
  int in = dup(0), out = dup(1);
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

  return return_with_reset_inout(in, out, 0);
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