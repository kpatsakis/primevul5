int ram_block_discard_range(RAMBlock *rb, uint64_t start, size_t length)
{
    int ret = -1;

    uint8_t *host_startaddr = rb->host + start;

    if (!QEMU_PTR_IS_ALIGNED(host_startaddr, rb->page_size)) {
        error_report("ram_block_discard_range: Unaligned start address: %p",
                     host_startaddr);
        goto err;
    }

    if ((start + length) <= rb->max_length) {
        bool need_madvise, need_fallocate;
        if (!QEMU_IS_ALIGNED(length, rb->page_size)) {
            error_report("ram_block_discard_range: Unaligned length: %zx",
                         length);
            goto err;
        }

        errno = ENOTSUP; /* If we are missing MADVISE etc */

        /* The logic here is messy;
         *    madvise DONTNEED fails for hugepages
         *    fallocate works on hugepages and shmem
         *    shared anonymous memory requires madvise REMOVE
         */
        need_madvise = (rb->page_size == qemu_host_page_size);
        need_fallocate = rb->fd != -1;
        if (need_fallocate) {
            /* For a file, this causes the area of the file to be zero'd
             * if read, and for hugetlbfs also causes it to be unmapped
             * so a userfault will trigger.
             */
#ifdef CONFIG_FALLOCATE_PUNCH_HOLE
            ret = fallocate(rb->fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE,
                            start, length);
            if (ret) {
                ret = -errno;
                error_report("ram_block_discard_range: Failed to fallocate "
                             "%s:%" PRIx64 " +%zx (%d)",
                             rb->idstr, start, length, ret);
                goto err;
            }
#else
            ret = -ENOSYS;
            error_report("ram_block_discard_range: fallocate not available/file"
                         "%s:%" PRIx64 " +%zx (%d)",
                         rb->idstr, start, length, ret);
            goto err;
#endif
        }
        if (need_madvise) {
            /* For normal RAM this causes it to be unmapped,
             * for shared memory it causes the local mapping to disappear
             * and to fall back on the file contents (which we just
             * fallocate'd away).
             */
#if defined(CONFIG_MADVISE)
            if (qemu_ram_is_shared(rb) && rb->fd < 0) {
                ret = madvise(host_startaddr, length, QEMU_MADV_REMOVE);
            } else {
                ret = madvise(host_startaddr, length, QEMU_MADV_DONTNEED);
            }
            if (ret) {
                ret = -errno;
                error_report("ram_block_discard_range: Failed to discard range "
                             "%s:%" PRIx64 " +%zx (%d)",
                             rb->idstr, start, length, ret);
                goto err;
            }
#else
            ret = -ENOSYS;
            error_report("ram_block_discard_range: MADVISE not available"
                         "%s:%" PRIx64 " +%zx (%d)",
                         rb->idstr, start, length, ret);
            goto err;
#endif
        }
        trace_ram_block_discard_range(rb->idstr, host_startaddr, length,
                                      need_madvise, need_fallocate, ret);
    } else {
        error_report("ram_block_discard_range: Overrun block '%s' (%" PRIu64
                     "/%zx/" RAM_ADDR_FMT")",
                     rb->idstr, start, length, rb->max_length);
    }

err:
    return ret;
}