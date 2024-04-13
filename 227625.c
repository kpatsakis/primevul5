static void msf2_phy_reset(MSF2EmacState *s)
{
    memset(&s->phy_regs[0], 0, sizeof(s->phy_regs));
    s->phy_regs[MII_BMCR] = 0x1140;
    s->phy_regs[MII_BMSR] = 0x7968;
    s->phy_regs[MII_PHYID1] = 0x0022;
    s->phy_regs[MII_PHYID2] = 0x1550;
    s->phy_regs[MII_ANAR] = 0x01E1;
    s->phy_regs[MII_ANLPAR] = 0xCDE1;

    msf2_phy_update_link(s);
}