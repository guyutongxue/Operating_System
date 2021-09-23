#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"


char read_argv[MAXARG][256];
char* new_argv[MAXARG];

int main(int argc, char** argv) {
  char in;
  int read_argv_cnt = 0;
  int current_argv_cnt = 0;
  for (int i = 1; i < argc; i++) {
    new_argv[i - 1] = argv[i];
  }

  while (read(0, &in, 1) > 0) {
    if (in == '\n') {
      if (current_argv_cnt > 0) {
        read_argv[read_argv_cnt][current_argv_cnt] = '\0';
        read_argv_cnt++;
      }
      // Execute
      for (int i = 0; i < read_argv_cnt; i++) {
        new_argv[argc + i - 1] = read_argv[i];
      }
      new_argv[argc + read_argv_cnt - 1] = 0;
      // debug
      // for (int i = 0; new_argv[i] != 0; i++) {
      //   printf("%s\n", new_argv[i]);
      // }
      if (fork() == 0) {
        // child
        exec(new_argv[0], new_argv);
      } else {
        // parent
        wait(0);
      }
    } else {
      // Parse input into read_argv
      if (in == ' ') {
        if (current_argv_cnt == 0) {
          continue;
        }
        read_argv[read_argv_cnt][current_argv_cnt] = '\0';
        read_argv_cnt++;
        current_argv_cnt = 0;
      } else {
        read_argv[read_argv_cnt][current_argv_cnt] = in;
        current_argv_cnt++;
      }
    }
  }
  exit(0);
}
