MemTxResult flatview_read_continue(FlatView *fv, hwaddr addr,
                                   MemTxAttrs attrs, void *ptr,
                                   hwaddr len, hwaddr addr1, hwaddr l,
                                   MemoryRegion *mr)
{
    uint8_t *ram_ptr;
    uint64_t val;
    MemTxResult result = MEMTX_OK;
    bool release_lock = false;
    uint8_t *buf = ptr;

    fuzz_dma_read_cb(addr, len, mr);
    for (;;) {
        if (!flatview_access_allowed(mr, attrs, addr1, l)) {
            result |= MEMTX_ACCESS_ERROR;
            /* Keep going. */
        } else if (!memory_access_is_direct(mr, false)) {
            /* I/O case */
            release_lock |= prepare_mmio_access(mr);
            l = memory_access_size(mr, l, addr1);
            result |= memory_region_dispatch_read(mr, addr1, &val,
                                                  size_memop(l), attrs);
            stn_he_p(buf, l, val);
        } else {
            /* RAM case */
            ram_ptr = qemu_ram_ptr_length(mr->ram_block, addr1, &l, false);
            memcpy(buf, ram_ptr, l);
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
        mr = flatview_translate(fv, addr, &addr1, &l, false, attrs);
    }

    return result;
}