static void sungem_mmio_greg_write(void *opaque, hwaddr addr, uint64_t val,
                                   unsigned size)
{
    SunGEMState *s = opaque;

    if (!(addr < 0x20) && !(addr >= 0x1000 && addr <= 0x1010)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "Write to unknown GREG register 0x%"HWADDR_PRIx"\n",
                      addr);
        return;
    }

    trace_sungem_mmio_greg_write(addr, val);

    /* Pre-write filter */
    switch (addr) {
    /* Read only registers */
    case GREG_SEBSTATE:
    case GREG_STAT:
    case GREG_STAT2:
    case GREG_PCIESTAT:
        return; /* No actual write */
    case GREG_IACK:
        val &= GREG_STAT_LATCH;
        s->gregs[GREG_STAT >> 2] &= ~val;
        sungem_eval_irq(s);
        return; /* No actual write */
    case GREG_PCIEMASK:
        val &= 0x7;
        break;
    }

    s->gregs[addr  >> 2] = val;

    /* Post write action */
    switch (addr) {
    case GREG_IMASK:
        /* Re-evaluate interrupt */
        sungem_eval_irq(s);
        break;
    case GREG_SWRST:
        switch (val & (GREG_SWRST_TXRST | GREG_SWRST_RXRST)) {
        case GREG_SWRST_RXRST:
            sungem_reset_rx(s);
            break;
        case GREG_SWRST_TXRST:
            sungem_reset_tx(s);
            break;
        case GREG_SWRST_RXRST | GREG_SWRST_TXRST:
            sungem_reset_all(s, false);
        }
        break;
    }
}