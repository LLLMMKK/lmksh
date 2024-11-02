#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
int main(int argc, char *argv[], char *envp[]) {
  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    printf("No such file.\n");
    exit(0);
  }
  char buf[512000];
  ssize_t rd = read(fd, buf, 512000);
  if (rd == -1) {
    printf("Failed to read the file.\n");
    exit(0);
  }
  printf("%s\n", buf);
  return 0;
}