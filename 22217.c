static uint64_t sungem_mmio_txdma_read(void *opaque, hwaddr addr, unsigned size)
{
    SunGEMState *s = opaque;
    uint32_t val;

    if (!(addr < 0x38) && !(addr >= 0x100 && addr <= 0x118)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "Read from unknown TXDMA register 0x%"HWADDR_PRIx"\n",
                      addr);
        return 0;
    }

    val = s->txdmaregs[addr >> 2];

    trace_sungem_mmio_txdma_read(addr, val);

    return val;
}