static void gem_phy_reset(CadenceGEMState *s)
{
    memset(&s->phy_regs[0], 0, sizeof(s->phy_regs));
    s->phy_regs[PHY_REG_CONTROL] = 0x1140;
    s->phy_regs[PHY_REG_STATUS] = 0x7969;
    s->phy_regs[PHY_REG_PHYID1] = 0x0141;
    s->phy_regs[PHY_REG_PHYID2] = 0x0CC2;
    s->phy_regs[PHY_REG_ANEGADV] = 0x01E1;
    s->phy_regs[PHY_REG_LINKPABIL] = 0xCDE1;
    s->phy_regs[PHY_REG_ANEGEXP] = 0x000F;
    s->phy_regs[PHY_REG_NEXTP] = 0x2001;
    s->phy_regs[PHY_REG_LINKPNEXTP] = 0x40E6;
    s->phy_regs[PHY_REG_100BTCTRL] = 0x0300;
    s->phy_regs[PHY_REG_1000BTSTAT] = 0x7C00;
    s->phy_regs[PHY_REG_EXTSTAT] = 0x3000;
    s->phy_regs[PHY_REG_PHYSPCFC_CTL] = 0x0078;
    s->phy_regs[PHY_REG_PHYSPCFC_ST] = 0x7C00;
    s->phy_regs[PHY_REG_EXT_PHYSPCFC_CTL] = 0x0C60;
    s->phy_regs[PHY_REG_LED] = 0x4100;
    s->phy_regs[PHY_REG_EXT_PHYSPCFC_CTL2] = 0x000A;
    s->phy_regs[PHY_REG_EXT_PHYSPCFC_ST] = 0x848B;

    phy_update_link(s);
}