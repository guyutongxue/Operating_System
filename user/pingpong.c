#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
  int parent2child[2];
  int child2parent[2];
  pipe(parent2child);
  pipe(child2parent);
  if (fork() == 0) {
    // child
    close(parent2child[1]);
    close(child2parent[0]);
    int pid = getpid();
    char received;
    read(parent2child[0], &received, 1);
    printf("%d: received ping\n", pid);
    write(child2parent[1], &received, 1);
    close(parent2child[0]);
    close(child2parent[1]);
    exit(0);
  } else {
    // parent
    close(parent2child[0]);
    close(child2parent[1]);
    int pid = getpid();
    char sent = 'G';
    write(parent2child[1], &sent, 1);
    read(child2parent[0], &sent, 1);
    printf("%d: received pong\n", pid);
    close(parent2child[1]);
    close(child2parent[0]);
    exit(0);
  }
}
