static void sungem_mmio_rxdma_write(void *opaque, hwaddr addr, uint64_t val,
                                    unsigned size)
{
    SunGEMState *s = opaque;

    if (!(addr <= 0x28) && !(addr >= 0x100 && addr <= 0x120)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "Write to unknown RXDMA register 0x%"HWADDR_PRIx"\n",
                      addr);
        return;
    }

    trace_sungem_mmio_rxdma_write(addr, val);

    /* Pre-write filter */
    switch (addr) {
    /* Read only registers */
    case RXDMA_DONE:
    case RXDMA_PCNT:
    case RXDMA_SMACHINE:
    case RXDMA_DPLOW:
    case RXDMA_DPHI:
    case RXDMA_FSZ:
    case RXDMA_FTAG:
        return; /* No actual write */
    }

    s->rxdmaregs[addr >> 2] = val;

    /* Post write action */
    switch (addr) {
    case RXDMA_KICK:
        trace_sungem_rx_kick(val);
        break;
    case RXDMA_CFG:
        sungem_update_masks(s);
        if ((s->macregs[MAC_RXCFG >> 2] & MAC_RXCFG_ENAB) != 0 &&
            (s->rxdmaregs[RXDMA_CFG >> 2] & RXDMA_CFG_ENABLE) != 0) {
            qemu_flush_queued_packets(qemu_get_queue(s->nic));
        }
        break;
    }
}