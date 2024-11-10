#ifndef REDIRECT_H
#define REDIRECT_H

#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

void redirect_stdin(char *argv) {
  int fp = open(argv, O_RDONLY);
  dup2(fp, STDIN_FILENO);
  close(fp);
}
void redirect_stdout(char *argv) {
  int fp = open(argv, O_WRONLY | O_CREAT, 0644);
  dup2(fp, STDOUT_FILENO);
  close(fp);
}
int return_with_reset_inout(int in, int out, int x) {
  dup2(in, STDIN_FILENO);
  dup2(out, STDOUT_FILENO);
  close(in);
  close(out);
  return x;
}
void check_redir(char **argv, int argc) {
  for (int i = 0; i + 1 < argc; i++) {
    if (!strcmp(argv[i], "<") || !strcmp(argv[i], ">")) {
      for (int j = i + 1; j + 1 < argc; j++) {
        std::swap(argv[j + 1], argv[j]);
        std::swap(argv[j], argv[j - 1]);
      }
      break;
    }
  }
}

#endif