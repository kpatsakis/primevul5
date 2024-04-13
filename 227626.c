static void msf2_phy_update_link(MSF2EmacState *s)
{
    /* Autonegotiation status mirrors link status. */
    if (qemu_get_queue(s->nic)->link_down) {
        s->phy_regs[MII_BMSR] &= ~(MII_BMSR_AN_COMP |
                                   MII_BMSR_LINK_ST);
    } else {
        s->phy_regs[MII_BMSR] |= (MII_BMSR_AN_COMP |
                                  MII_BMSR_LINK_ST);
    }
}