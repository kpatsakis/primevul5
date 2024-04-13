int qemu_ram_resize(RAMBlock *block, ram_addr_t newsize, Error **errp)
{
    const ram_addr_t oldsize = block->used_length;
    const ram_addr_t unaligned_size = newsize;

    assert(block);

    newsize = HOST_PAGE_ALIGN(newsize);

    if (block->used_length == newsize) {
        /*
         * We don't have to resize the ram block (which only knows aligned
         * sizes), however, we have to notify if the unaligned size changed.
         */
        if (unaligned_size != memory_region_size(block->mr)) {
            memory_region_set_size(block->mr, unaligned_size);
            if (block->resized) {
                block->resized(block->idstr, unaligned_size, block->host);
            }
        }
        return 0;
    }

    if (!(block->flags & RAM_RESIZEABLE)) {
        error_setg_errno(errp, EINVAL,
                         "Size mismatch: %s: 0x" RAM_ADDR_FMT
                         " != 0x" RAM_ADDR_FMT, block->idstr,
                         newsize, block->used_length);
        return -EINVAL;
    }

    if (block->max_length < newsize) {
        error_setg_errno(errp, EINVAL,
                         "Size too large: %s: 0x" RAM_ADDR_FMT
                         " > 0x" RAM_ADDR_FMT, block->idstr,
                         newsize, block->max_length);
        return -EINVAL;
    }

    /* Notify before modifying the ram block and touching the bitmaps. */
    if (block->host) {
        ram_block_notify_resize(block->host, oldsize, newsize);
    }

    cpu_physical_memory_clear_dirty_range(block->offset, block->used_length);
    block->used_length = newsize;
    cpu_physical_memory_set_dirty_range(block->offset, block->used_length,
                                        DIRTY_CLIENTS_ALL);
    memory_region_set_size(block->mr, unaligned_size);
    if (block->resized) {
        block->resized(block->idstr, unaligned_size, block->host);
    }
    return 0;
}