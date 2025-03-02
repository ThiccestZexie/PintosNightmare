#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

struct alloc_info
{
  void *page_base;
  size_t alloc_size;
};

size_t align(size_t sz, size_t alignment)
{
  return (sz + alignment - 1) & ~(alignment - 1);
}

void *malloc(size_t size)
{
  // Minimum alignment for data structures.
  size = align(size, 32);

  size_t pagesize = sysconf(_SC_PAGE_SIZE);
  size_t round_size = align(size + sizeof(struct alloc_info), pagesize) + pagesize;
  void *mem = mmap(NULL, round_size + pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  // Add a "guard page" that will fault.
  mprotect((char *)mem + round_size, pagesize, PROT_NONE);

  memset(mem, 0xCC, round_size);

  struct alloc_info *info = (void *)((char *)mem + round_size - size - sizeof(struct alloc_info));
  info->page_base = mem;
  info->alloc_size = round_size;

  return (char *)info + sizeof(struct alloc_info);
}

void free(void *mem)
{
  struct alloc_info *info = (void *)((char *)mem - sizeof(struct alloc_info));
  munmap(info->page_base, info->alloc_size);
}
