RAMBlock *qemu_ram_alloc_from_file(ram_addr_t size, MemoryRegion *mr,
                                   uint32_t ram_flags, const char *mem_path,
                                   bool readonly, Error **errp)
{
    int fd;
    bool created;
    RAMBlock *block;

    fd = file_ram_open(mem_path, memory_region_name(mr), readonly, &created,
                       errp);
    if (fd < 0) {
        return NULL;
    }

    block = qemu_ram_alloc_from_fd(size, mr, ram_flags, fd, 0, readonly, errp);
    if (!block) {
        if (created) {
            unlink(mem_path);
        }
        close(fd);
        return NULL;
    }

    return block;
}