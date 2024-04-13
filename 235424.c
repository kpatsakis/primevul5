void qemu_ram_remap(ram_addr_t addr, ram_addr_t length)
{
    RAMBlock *block;
    ram_addr_t offset;
    int flags;
    void *area, *vaddr;

    RAMBLOCK_FOREACH(block) {
        offset = addr - block->offset;
        if (offset < block->max_length) {
            vaddr = ramblock_ptr(block, offset);
            if (block->flags & RAM_PREALLOC) {
                ;
            } else if (xen_enabled()) {
                abort();
            } else {
                flags = MAP_FIXED;
                flags |= block->flags & RAM_SHARED ?
                         MAP_SHARED : MAP_PRIVATE;
                flags |= block->flags & RAM_NORESERVE ? MAP_NORESERVE : 0;
                if (block->fd >= 0) {
                    area = mmap(vaddr, length, PROT_READ | PROT_WRITE,
                                flags, block->fd, offset);
                } else {
                    flags |= MAP_ANONYMOUS;
                    area = mmap(vaddr, length, PROT_READ | PROT_WRITE,
                                flags, -1, 0);
                }
                if (area != vaddr) {
                    error_report("Could not remap addr: "
                                 RAM_ADDR_FMT "@" RAM_ADDR_FMT "",
                                 length, addr);
                    exit(1);
                }
                memory_try_enable_merging(vaddr, length);
                qemu_ram_setup_dump(vaddr, length);
            }
        }
    }
}