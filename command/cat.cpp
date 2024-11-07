#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
int main(int argc, char *argv[], char *envp[]) {

  for (int i = 1; argv[i] != NULL; i++) {
    if (strcmp(argv[i], "<") && strcmp(argv[i], ">")) {
      int fd = open(argv[i], O_RDONLY);
      if (fd == -1)
        printf("No file named %s.\n", argv[i]);
      else {
        char buf[512000];
        ssize_t rd = read(fd, buf, 512000);
        close(fd);
        if (rd == -1)
          printf("Failed to read the file %s.\n", argv[i]);
        else
          printf("%s\n", buf);
      }
    }
  }

  exit(0);
}