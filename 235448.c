static void *file_ram_alloc(RAMBlock *block,
                            ram_addr_t memory,
                            int fd,
                            bool readonly,
                            bool truncate,
                            off_t offset,
                            Error **errp)
{
    uint32_t qemu_map_flags;
    void *area;

    block->page_size = qemu_fd_getpagesize(fd);
    if (block->mr->align % block->page_size) {
        error_setg(errp, "alignment 0x%" PRIx64
                   " must be multiples of page size 0x%zx",
                   block->mr->align, block->page_size);
        return NULL;
    } else if (block->mr->align && !is_power_of_2(block->mr->align)) {
        error_setg(errp, "alignment 0x%" PRIx64
                   " must be a power of two", block->mr->align);
        return NULL;
    }
    block->mr->align = MAX(block->page_size, block->mr->align);
#if defined(__s390x__)
    if (kvm_enabled()) {
        block->mr->align = MAX(block->mr->align, QEMU_VMALLOC_ALIGN);
    }
#endif

    if (memory < block->page_size) {
        error_setg(errp, "memory size 0x" RAM_ADDR_FMT " must be equal to "
                   "or larger than page size 0x%zx",
                   memory, block->page_size);
        return NULL;
    }

    memory = ROUND_UP(memory, block->page_size);

    /*
     * ftruncate is not supported by hugetlbfs in older
     * hosts, so don't bother bailing out on errors.
     * If anything goes wrong with it under other filesystems,
     * mmap will fail.
     *
     * Do not truncate the non-empty backend file to avoid corrupting
     * the existing data in the file. Disabling shrinking is not
     * enough. For example, the current vNVDIMM implementation stores
     * the guest NVDIMM labels at the end of the backend file. If the
     * backend file is later extended, QEMU will not be able to find
     * those labels. Therefore, extending the non-empty backend file
     * is disabled as well.
     */
    if (truncate && ftruncate(fd, memory)) {
        perror("ftruncate");
    }

    qemu_map_flags = readonly ? QEMU_MAP_READONLY : 0;
    qemu_map_flags |= (block->flags & RAM_SHARED) ? QEMU_MAP_SHARED : 0;
    qemu_map_flags |= (block->flags & RAM_PMEM) ? QEMU_MAP_SYNC : 0;
    qemu_map_flags |= (block->flags & RAM_NORESERVE) ? QEMU_MAP_NORESERVE : 0;
    area = qemu_ram_mmap(fd, memory, block->mr->align, qemu_map_flags, offset);
    if (area == MAP_FAILED) {
        error_setg_errno(errp, errno,
                         "unable to map backing store for guest RAM");
        return NULL;
    }

    block->fd = fd;
    return area;
}