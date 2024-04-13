static uint16_t read_from_phy(MSF2EmacState *s)
{
    uint8_t reg_addr = s->regs[R_MII_ADDR] & R_MII_ADDR_REGADDR_MASK;
    uint8_t phy_addr = (s->regs[R_MII_ADDR] >> R_MII_ADDR_PHYADDR_SHIFT) &
                       R_MII_ADDR_REGADDR_MASK;

    if (phy_addr == PHYADDR) {
        return s->phy_regs[reg_addr];
    } else {
        return 0xFFFF;
    }
}