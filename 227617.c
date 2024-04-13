static void write_to_phy(MSF2EmacState *s)
{
    uint8_t reg_addr = s->regs[R_MII_ADDR] & R_MII_ADDR_REGADDR_MASK;
    uint8_t phy_addr = (s->regs[R_MII_ADDR] >> R_MII_ADDR_PHYADDR_SHIFT) &
                       R_MII_ADDR_REGADDR_MASK;
    uint16_t data = s->regs[R_MII_CTL] & 0xFFFF;

    if (phy_addr != PHYADDR) {
        return;
    }

    switch (reg_addr) {
    case MII_BMCR:
        if (data & MII_BMCR_RESET) {
            /* Phy reset */
            msf2_phy_reset(s);
            data &= ~MII_BMCR_RESET;
        }
        if (data & MII_BMCR_AUTOEN) {
            /* Complete autonegotiation immediately */
            data &= ~MII_BMCR_AUTOEN;
            s->phy_regs[MII_BMSR] |= MII_BMSR_AN_COMP;
        }
        break;
    }

    s->phy_regs[reg_addr] = data;
}