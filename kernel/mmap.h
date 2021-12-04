#ifndef MMAP_H
#define MMAP_H

#include "types.h"

struct vma {
  uint64 start;
  uint64 end;
  int prot;
  int flags;
  struct file* file;
  int offset;
};

#define NVMA 16
#define VMA_START_INIT (MAXVA - 2 * PGSIZE) // TRAMPOLINE & TRAPFRAME
#define VMA_START(p) ((p)->nvma == 0 ? VMA_START_INIT : (p)->vma[(p)->nvma - 1].start)

#endif
