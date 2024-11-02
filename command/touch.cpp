#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char *argv[], char *envp[]) {
  mode_t mod = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = creat(argv[1], mod);
  return 0;
}
