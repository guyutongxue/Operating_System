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

int page_fault_handler(uint64 va) {
  struct proc* p = myproc();
  va = PGROUNDDOWN(va);
  struct vma* current = 0;
  for (int i = 0; i < p->nvma; i++) {
    struct vma* v = &p->vma[i];
    if (v->start <= va && va < v->end) {
      current = v;
      break;
    }
  }
  if (current == 0) {
    printf("page fault: no vma for %p\n", va);
    return -1;
  }
  char* mem = kalloc();
  memset(mem, 0, PGSIZE);
  if (mem == 0) {
    printf("page fault: no memory\n");
    return -1;
  }
  int prot = 0;
  if (current->prot & PROT_READ) prot |= PTE_R;
  if (current->prot & PROT_WRITE) prot |= PTE_W;
  if (mappages(p->pagetable, va, PGSIZE, (uint64)mem, prot | PTE_U | PTE_X) < 0) {
    printf("page fault: mappages failed\n");
    kfree(mem);
    return -1;
  }
  struct file* f = current->file;
  int offset = va - current->start + current->offset;
  int size = f->ip->size - offset;
  if (size > PGSIZE) size = PGSIZE;
  ilock(f->ip);
  if (readi(f->ip, 1, va, offset, size) != size) {
    iunlock(f->ip);
    printf("page fault: readi failed\n");
    return -1;
  }
  iunlock(f->ip);
  return 0;
}

uint64 sys_munmap(void) {
  return -1;
}
