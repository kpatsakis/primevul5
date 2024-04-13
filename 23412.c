static uint64_t gem_read(void *opaque, hwaddr offset, unsigned size)
{
    CadenceGEMState *s;
    uint32_t retval;
    s = (CadenceGEMState *)opaque;

    offset >>= 2;
    retval = s->regs[offset];

    DB_PRINT("offset: 0x%04x read: 0x%08x\n", (unsigned)offset*4, retval);

    switch (offset) {
    case GEM_ISR:
        DB_PRINT("lowering irqs on ISR read\n");
        /* The interrupts get updated at the end of the function. */
        break;
    case GEM_PHYMNTNC:
        if (retval & GEM_PHYMNTNC_OP_R) {
            uint32_t phy_addr, reg_num;

            phy_addr = (retval & GEM_PHYMNTNC_ADDR) >> GEM_PHYMNTNC_ADDR_SHFT;
            if (phy_addr == s->phy_addr) {
                reg_num = (retval & GEM_PHYMNTNC_REG) >> GEM_PHYMNTNC_REG_SHIFT;
                retval &= 0xFFFF0000;
                retval |= gem_phy_read(s, reg_num);
            } else {
                retval |= 0xFFFF; /* No device at this address */
            }
        }
        break;
    }

    /* Squash read to clear bits */
    s->regs[offset] &= ~(s->regs_rtc[offset]);

    /* Do not provide write only bits */
    retval &= ~(s->regs_wo[offset]);

    DB_PRINT("0x%08x\n", retval);
    gem_update_int_status(s);
    return retval;
}