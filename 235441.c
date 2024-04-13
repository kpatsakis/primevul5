static inline MemTxResult address_space_write_rom_internal(AddressSpace *as,
                                                           hwaddr addr,
                                                           MemTxAttrs attrs,
                                                           const void *ptr,
                                                           hwaddr len,
                                                           enum write_rom_type type)
{
    hwaddr l;
    uint8_t *ram_ptr;
    hwaddr addr1;
    MemoryRegion *mr;
    const uint8_t *buf = ptr;

    RCU_READ_LOCK_GUARD();
    while (len > 0) {
        l = len;
        mr = address_space_translate(as, addr, &addr1, &l, true, attrs);

        if (!(memory_region_is_ram(mr) ||
              memory_region_is_romd(mr))) {
            l = memory_access_size(mr, l, addr1);
        } else {
            /* ROM/RAM case */
            ram_ptr = qemu_map_ram_ptr(mr->ram_block, addr1);
            switch (type) {
            case WRITE_DATA:
                memcpy(ram_ptr, buf, l);
                invalidate_and_set_dirty(mr, addr1, l);
                break;
            case FLUSH_CACHE:
                flush_idcache_range((uintptr_t)ram_ptr, (uintptr_t)ram_ptr, l);
                break;
            }
        }
        len -= l;
        buf += l;
        addr += l;
    }
    return MEMTX_OK;
}