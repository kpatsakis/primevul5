static uint64_t sungem_mmio_pcs_read(void *opaque, hwaddr addr, unsigned size)
{
    SunGEMState *s = opaque;
    uint32_t val;

    if (!(addr <= 0x18) && !(addr >= 0x50 && addr <= 0x5c)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "Read from unknown PCS register 0x%"HWADDR_PRIx"\n",
                      addr);
        return 0;
    }

    val = s->pcsregs[addr >> 2];

    trace_sungem_mmio_pcs_read(addr, val);

    return val;
}