#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fcntl.h"
#include "file.h"
#include "proc.h"


uint64 mmap(int length, int prot, int flags, struct file* file) {
  struct proc* p = myproc();

  // find a free vma
  if (p->nvma == NVMA) {
    return -1;
  }
  uint64 start = VMA_START(p);
  struct vma* current = &p->vma[p->nvma];
  p->nvma++;
  uint64 vm_end = PGROUNDDOWN(start);
  uint64 vm_start = PGROUNDDOWN(start - length);
  current->start = vm_start;
  current->end = vm_end;
  current->prot = prot;
  current->flags = flags;
  current->file = filedup(file);
  current->offset = 0;
  return vm_start;
}

uint64 sys_mmap(void) {
  uint64 vaddr;
  int len, prot, flags, offset;
  struct file *f;

  if (argaddr(0, &vaddr) < 0 || argint(1, &len) < 0 || argint(2, &prot) < 0 || 
      argint(3, &flags) < 0 || argfd(4, 0, &f) < 0 || argint(5, &offset) < 0) {
    return -1;
  }

  if (!f->writable && (prot & PROT_WRITE) && (flags & MAP_SHARED)) {
    return -1;
  }
  if (vaddr != 0 || offset != 0) {
    return -1;
  }

  return mmap(len, prot, flags, f);
}

uint64 sys_munmap(void) {
  return -1;
}
