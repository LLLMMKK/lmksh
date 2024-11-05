#include <cstdio>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>
int main(int argc, char *argv[], char *envp[]) {

  for (int i = 1; argv[i] != NULL; i++) {
    int fd = open(argv[i], O_CREAT | O_WRONLY,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    close(fd);
    utime(argv[i], NULL);
  }
  return 0;
}
