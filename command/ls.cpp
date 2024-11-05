#include <algorithm>
#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char *argv[], char *envp[]) {

  bool flaga = 0, flagl = 0;
  for (int i = 1; argv[i] != NULL; i++) {
    if (argv[i][0] == '-') {
      for (int j = 1; j < strlen(argv[i]); j++) {
        if (argv[i][j] == 'a')
          flaga = 1;
        if (argv[i][j] == 'l')
          flagl = 1;
      }
    }
  }

  char *sdir = getenv("PWD");
  for (int i = 1; argv[i] != NULL; i++) {
    struct stat info;
    if (stat(argv[i], &info) == 0) {
      if (info.st_mode & S_IFDIR) {
        sdir = argv[i];
        break;
      }
    }
  }
  DIR *dir = opendir(sdir);

  char *names[114514];
  int namescnt = 0;
  dirent *tmp;
  while ((tmp = readdir(dir)) != NULL) {
    if (tmp->d_name[0] == '.' && !flaga)
      continue;
    names[++namescnt] = tmp->d_name;
  }
  std::sort(names + 1, names + namescnt + 1, [](const char *a, const char *b) {
    if (a[0] == '.' && b[0] != '.')
      return true;
    if (a[0] != '.' && b[0] == '.')
      return false;
    return strcmp(a, b) < 0;
  });

  for (int i = 1; i <= namescnt; i++) {
    if (flagl) {
      struct stat info;
      stat(names[i], &info);

      mode_t mode = info.st_mode;
      switch (mode & S_IFMT) {
      case S_IFREG:
        printf("-");
        break;
      case S_IFDIR:
        printf("d");
        break;
      case S_IFLNK:
        printf("l");
        break;
      case S_IFCHR:
        printf("c");
        break;
      case S_IFBLK:
        printf("b");
        break;
      case S_IFIFO:
        printf("p");
        break;
      case S_IFSOCK:
        printf("s");
        break;
      default:
        printf("U");
        break;
      }
      printf((mode & S_IRUSR) ? "r" : "-");
      printf((mode & S_IWUSR) ? "w" : "-");
      printf((mode & S_IXUSR) ? "x" : "-");
      printf((mode & S_IRGRP) ? "r" : "-");
      printf((mode & S_IWGRP) ? "w" : "-");
      printf((mode & S_IXGRP) ? "x" : "-");
      printf((mode & S_IROTH) ? "r" : "-");
      printf((mode & S_IWOTH) ? "w" : "-");
      printf((mode & S_IXOTH) ? "x" : "-");

      printf(" %lu", info.st_nlink);
      struct passwd *pw = getpwuid(info.st_uid);
      struct group *gr = getgrgid(info.st_gid);
      printf(" %s", pw->pw_name);
      printf(" %s", gr->gr_name);
      printf(" %lu", info.st_size);
      char timebuf[80];
      struct tm *timeinfo;
      timeinfo = localtime(&info.st_mtim.tv_sec);
      strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", timeinfo);
      printf(" %s", timebuf);
      printf(" %s\n", names[i]);

    } else {
      printf("%s    ", names[i]);
    }
  }

  printf("\n");

  closedir(dir);

  exit(0);
}
