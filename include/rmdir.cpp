#include <cstdio>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char *argv[], char *envp[]) {
  rmdir(argv[1]);
  return 0;
}