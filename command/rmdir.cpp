#include <cstdio>
#include <cstdlib>
#include <unistd.h>
int main(int argc, char *argv[]) {
  for (int i = 1; argv[i] != NULL; i++)
    rmdir(argv[i]);
  exit(0);
}