#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
int main(int argc, char *argv[], char *envp[]) {

  for (int i = 0; argv[i] != NULL; i++)
    fprintf(stderr, "%s#", argv[i]);

  int isnull = 1;
  for (int i = 1; argv[i] != NULL; i++) {
    if (!strcmp(argv[i], "<") || !strcmp(argv[i], ">")) {
      i++;
      continue;
    }
    if (!strcmp(argv[i], "\n"))
      continue;

    isnull = 0;

    if (!strcmp(argv[i], "-")) {
      char buffer[8192];
      ssize_t bytesRead;
      while ((bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
      }
    } else {
      int fd = open(argv[i], O_RDONLY);
      if (fd == -1)
        printf("No file named %s.", argv[i]);
      else {
        char buf[512000];
        ssize_t rd = read(fd, buf, 512000);
        close(fd);
        if (rd == -1)
          printf("Failed to read the file %s.", argv[i]);
        else
          printf("%s", buf);
      }
    }
  }
  if (isnull) {
    char buffer[8192];
    ssize_t bytesRead;
    while ((bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {
      buffer[bytesRead] = '\0';
      printf("%s", buffer);
    }
  }
  exit(0);
}