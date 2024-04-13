static MemTxResult flatview_write_continue(FlatView *fv, hwaddr addr,
                                           MemTxAttrs attrs,
                                           const void *ptr,
                                           hwaddr len, hwaddr addr1,
                                           hwaddr l, MemoryRegion *mr)
{
    uint8_t *ram_ptr;
    uint64_t val;
    MemTxResult result = MEMTX_OK;
    bool release_lock = false;
    const uint8_t *buf = ptr;

    for (;;) {
        if (!flatview_access_allowed(mr, attrs, addr1, l)) {
            result |= MEMTX_ACCESS_ERROR;
            /* Keep going. */
        } else if (!memory_access_is_direct(mr, true)) {
            release_lock |= prepare_mmio_access(mr);
            l = memory_access_size(mr, l, addr1);
            /* XXX: could force current_cpu to NULL to avoid
               potential bugs */
            val = ldn_he_p(buf, l);
            result |= memory_region_dispatch_write(mr, addr1, val,
                                                   size_memop(l), attrs);
        } else {
            /* RAM case */
            ram_ptr = qemu_ram_ptr_length(mr->ram_block, addr1, &l, false);
            memcpy(ram_ptr, buf, l);
            invalidate_and_set_dirty(mr, addr1, l);
        }

        if (release_lock) {
            qemu_mutex_unlock_iothread();
            release_lock = false;
        }

        len -= l;
        buf += l;
        addr += l;

        if (!len) {
            break;
        }

        l = len;
        mr = flatview_translate(fv, addr, &addr1, &l, true, attrs);
    }

    return result;
}