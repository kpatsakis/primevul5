  static void *operator new(size_t size, MEM_ROOT *mem_root)
  {
    return alloc_root(mem_root, size);
  }