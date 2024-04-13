static uint64_t sungem_mmio_mif_read(void *opaque, hwaddr addr, unsigned size)
{
    SunGEMState *s = opaque;
    uint32_t val;

    if (!(addr <= 0x1c)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "Read from unknown MIF register 0x%"HWADDR_PRIx"\n",
                      addr);
        return 0;
    }

    val = s->mifregs[addr >> 2];

    trace_sungem_mmio_mif_read(addr, val);

    return val;
}