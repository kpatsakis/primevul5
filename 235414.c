RAMBlock *qemu_ram_alloc_from_fd(ram_addr_t size, MemoryRegion *mr,
                                 uint32_t ram_flags, int fd, off_t offset,
                                 bool readonly, Error **errp)
{
    RAMBlock *new_block;
    Error *local_err = NULL;
    int64_t file_size, file_align;

    /* Just support these ram flags by now. */
    assert((ram_flags & ~(RAM_SHARED | RAM_PMEM | RAM_NORESERVE |
                          RAM_PROTECTED)) == 0);

    if (xen_enabled()) {
        error_setg(errp, "-mem-path not supported with Xen");
        return NULL;
    }

    if (kvm_enabled() && !kvm_has_sync_mmu()) {
        error_setg(errp,
                   "host lacks kvm mmu notifiers, -mem-path unsupported");
        return NULL;
    }

    size = HOST_PAGE_ALIGN(size);
    file_size = get_file_size(fd);
    if (file_size > 0 && file_size < size) {
        error_setg(errp, "backing store size 0x%" PRIx64
                   " does not match 'size' option 0x" RAM_ADDR_FMT,
                   file_size, size);
        return NULL;
    }

    file_align = get_file_align(fd);
    if (file_align > 0 && file_align > mr->align) {
        error_setg(errp, "backing store align 0x%" PRIx64
                   " is larger than 'align' option 0x%" PRIx64,
                   file_align, mr->align);
        return NULL;
    }

    new_block = g_malloc0(sizeof(*new_block));
    new_block->mr = mr;
    new_block->used_length = size;
    new_block->max_length = size;
    new_block->flags = ram_flags;
    new_block->host = file_ram_alloc(new_block, size, fd, readonly,
                                     !file_size, offset, errp);
    if (!new_block->host) {
        g_free(new_block);
        return NULL;
    }

    ram_block_add(new_block, &local_err);
    if (local_err) {
        g_free(new_block);
        error_propagate(errp, local_err);
        return NULL;
    }
    return new_block;

}