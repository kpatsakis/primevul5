static uint64_t sungem_mmio_greg_read(void *opaque, hwaddr addr, unsigned size)
{
    SunGEMState *s = opaque;
    uint32_t val;

    if (!(addr < 0x20) && !(addr >= 0x1000 && addr <= 0x1010)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "Read from unknown GREG register 0x%"HWADDR_PRIx"\n",
                      addr);
        return 0;
    }

    val = s->gregs[addr >> 2];

    trace_sungem_mmio_greg_read(addr, val);

    switch (addr) {
    case GREG_STAT:
        /* Side effect, clear bottom 7 bits */
        s->gregs[GREG_STAT >> 2] &= ~GREG_STAT_LATCH;
        sungem_eval_irq(s);

        /* Inject TX completion in returned value */
        val = (val & ~GREG_STAT_TXNR) |
                (s->txdmaregs[TXDMA_TXDONE >> 2] << GREG_STAT_TXNR_SHIFT);
        break;
    case GREG_STAT2:
        /* Return the status reg without side effect
         * (and inject TX completion in returned value)
         */
        val = (s->gregs[GREG_STAT >> 2] & ~GREG_STAT_TXNR) |
              (s->txdmaregs[TXDMA_TXDONE >> 2] << GREG_STAT_TXNR_SHIFT);
        break;
    }

    return val;
}