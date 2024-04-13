static void sungem_mmio_txdma_write(void *opaque, hwaddr addr, uint64_t val,
                                    unsigned size)
{
    SunGEMState *s = opaque;

    if (!(addr < 0x38) && !(addr >= 0x100 && addr <= 0x118)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "Write to unknown TXDMA register 0x%"HWADDR_PRIx"\n",
                      addr);
        return;
    }

    trace_sungem_mmio_txdma_write(addr, val);

    /* Pre-write filter */
    switch (addr) {
    /* Read only registers */
    case TXDMA_TXDONE:
    case TXDMA_PCNT:
    case TXDMA_SMACHINE:
    case TXDMA_DPLOW:
    case TXDMA_DPHI:
    case TXDMA_FSZ:
    case TXDMA_FTAG:
        return; /* No actual write */
    }

    s->txdmaregs[addr >> 2] = val;

    /* Post write action */
    switch (addr) {
    case TXDMA_KICK:
        sungem_tx_kick(s);
        break;
    case TXDMA_CFG:
        sungem_update_masks(s);
        break;
    }
}