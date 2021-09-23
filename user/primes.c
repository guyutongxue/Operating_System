#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void solve(void) {
  int prime;
  if (read(0, &prime, sizeof(prime)) <= 0) {
    return;
  }
  // print first read
  printf("prime %d\n", prime);

  int p[2];
  pipe(p);

  if (fork() == 0) {
    // next child
    close(p[1]);

    // redirect stdin to pipe
    close(0);
    dup(p[0]);
    close(p[0]);

    solve();
  } else {
    // this process
    close(p[0]);

    // filter other reads
    // then write to childs
    int r;
    while (read(0, &r, sizeof(r)) > 0) {
      if (r % prime != 0) {
        write(p[1], &r, sizeof(r));
      }
    }
    close(p[1]);
    wait(0);
  }
}

int main(void) {
  int firstPipe[2];
  pipe(firstPipe);
  if (fork() == 0) {
    // child
    close(firstPipe[1]);
    
    // redirect stdin to pipe
    close(0);
    dup(firstPipe[0]);
    close(firstPipe[0]);

    solve();
    exit(0);
  } else {
    // parent
    // init first pipe
    close(firstPipe[0]);
    for (int i = 2; i <= 35; i++) {
      write(firstPipe[1], &i, sizeof(i));
    }
    close(firstPipe[1]);
    wait(0);
    exit(0);
  }
}
