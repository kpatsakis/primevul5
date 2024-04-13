void *address_space_map(AddressSpace *as,
                        hwaddr addr,
                        hwaddr *plen,
                        bool is_write,
                        MemTxAttrs attrs)
{
    hwaddr len = *plen;
    hwaddr l, xlat;
    MemoryRegion *mr;
    void *ptr;
    FlatView *fv;

    if (len == 0) {
        return NULL;
    }

    l = len;
    RCU_READ_LOCK_GUARD();
    fv = address_space_to_flatview(as);
    mr = flatview_translate(fv, addr, &xlat, &l, is_write, attrs);

    if (!memory_access_is_direct(mr, is_write)) {
        if (qatomic_xchg(&bounce.in_use, true)) {
            *plen = 0;
            return NULL;
        }
        /* Avoid unbounded allocations */
        l = MIN(l, TARGET_PAGE_SIZE);
        bounce.buffer = qemu_memalign(TARGET_PAGE_SIZE, l);
        bounce.addr = addr;
        bounce.len = l;

        memory_region_ref(mr);
        bounce.mr = mr;
        if (!is_write) {
            flatview_read(fv, addr, MEMTXATTRS_UNSPECIFIED,
                               bounce.buffer, l);
        }

        *plen = l;
        return bounce.buffer;
    }


    memory_region_ref(mr);
    *plen = flatview_extend_translation(fv, addr, len, mr, xlat,
                                        l, is_write, attrs);
    fuzz_dma_read_cb(addr, *plen, mr);
    ptr = qemu_ram_ptr_length(mr->ram_block, xlat, plen, true);

    return ptr;
}