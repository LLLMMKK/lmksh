#include <cstdio>
#include <dirent.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char *argv[], char *envp[]) {
  DIR *dir = opendir(getenv("PWD"));

  dirent *tmp;
  while ((tmp = readdir(dir)) != NULL) {
    printf("%s     ", tmp->d_name);
  }
  printf("\n");
  return 0;
}