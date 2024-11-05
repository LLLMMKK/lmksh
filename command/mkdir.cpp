#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int main(int argc, char *argv[], char *envp[]) {
  mode_t mod = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  for (int i = 1; argv[i] != NULL; i++) {
    mkdir(argv[i], mod);
  }
  exit(0);
}