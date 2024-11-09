#ifndef REDIRECT_H
#define REDIRECT_H

#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

void redirect_stdin(char *argv);
void redirect_stdout(char *argv);
int return_with_reset_inout(int in, int out, int x);
void check_redir(char **argv, int argc);

#endif