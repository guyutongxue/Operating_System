#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

const char* target = "";

char* fmtname(char* path) {
  static char buf[DIRSIZ + 1];
  char* p;

  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if (strlen(p) >= DIRSIZ)
    return p;

  memmove(buf, p, strlen(p) + 1);
  return buf;
}

void find(char* path) {
  char buf[512];
  char* p = 0;

  struct dirent de;
  struct stat st;

  int fd;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    return;
  }

  switch (st.type) {
    case T_FILE:
      if (strcmp(target, fmtname(path)) == 0) {
        printf("%s\n", path);
      }
      break;
    case T_DIR:
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        fprintf(2, "find: path too long\n");
        break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
          continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = '\0';
        if (strlen(de.name) == 1 && de.name[0] == '.')
          continue;
        if (strlen(de.name) == 2 && de.name[0] == '.' && de.name[1] == '.')
          continue;
        find(buf);
      }
  }
  close(fd);
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: find path filename\n");
    exit(1);
  }

  target = argv[2];
  find(argv[1]);
  exit(0);
}
