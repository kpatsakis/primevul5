int cpu_memory_rw_debug(CPUState *cpu, vaddr addr,
                        void *ptr, size_t len, bool is_write)
{
    hwaddr phys_addr;
    vaddr l, page;
    uint8_t *buf = ptr;

    cpu_synchronize_state(cpu);
    while (len > 0) {
        int asidx;
        MemTxAttrs attrs;
        MemTxResult res;

        page = addr & TARGET_PAGE_MASK;
        phys_addr = cpu_get_phys_page_attrs_debug(cpu, page, &attrs);
        asidx = cpu_asidx_from_attrs(cpu, attrs);
        /* if no physical page mapped, return an error */
        if (phys_addr == -1)
            return -1;
        l = (page + TARGET_PAGE_SIZE) - addr;
        if (l > len)
            l = len;
        phys_addr += (addr & ~TARGET_PAGE_MASK);
        if (is_write) {
            res = address_space_write_rom(cpu->cpu_ases[asidx].as, phys_addr,
                                          attrs, buf, l);
        } else {
            res = address_space_read(cpu->cpu_ases[asidx].as, phys_addr,
                                     attrs, buf, l);
        }
        if (res != MEMTX_OK) {
            return -1;
        }
        len -= l;
        buf += l;
        addr += l;
    }
    return 0;
}