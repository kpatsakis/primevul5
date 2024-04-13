static void sungem_mmio_pcs_write(void *opaque, hwaddr addr, uint64_t val,
                                  unsigned size)
{
    SunGEMState *s = opaque;

    if (!(addr <= 0x18) && !(addr >= 0x50 && addr <= 0x5c)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "Write to unknown PCS register 0x%"HWADDR_PRIx"\n",
                      addr);
        return;
    }

    trace_sungem_mmio_pcs_write(addr, val);

    /* Pre-write filter */
    switch (addr) {
    /* Read only registers */
    case PCS_MIISTAT:
    case PCS_ISTAT:
    case PCS_SSTATE:
        return; /* No actual write */
    }

    s->pcsregs[addr >> 2] = val;
}