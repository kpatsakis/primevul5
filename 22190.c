static void sungem_mmio_mif_write(void *opaque, hwaddr addr, uint64_t val,
                                  unsigned size)
{
    SunGEMState *s = opaque;

    if (!(addr <= 0x1c)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "Write to unknown MIF register 0x%"HWADDR_PRIx"\n",
                      addr);
        return;
    }

    trace_sungem_mmio_mif_write(addr, val);

    /* Pre-write filter */
    switch (addr) {
    /* Read only registers */
    case MIF_STATUS:
    case MIF_SMACHINE:
        return; /* No actual write */
    case MIF_CFG:
        /* Maintain the RO MDI bits to advertize an MDIO PHY on MDI0 */
        val &= ~MIF_CFG_MDI1;
        val |= MIF_CFG_MDI0;
        break;
    }

    s->mifregs[addr >> 2] = val;

    /* Post write action */
    switch (addr) {
    case MIF_FRAME:
        s->mifregs[addr >> 2] = sungem_mii_op(s, val);
        break;
    }
}