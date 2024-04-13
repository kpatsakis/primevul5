static void gem_phy_write(CadenceGEMState *s, unsigned reg_num, uint16_t val)
{
    DB_PRINT("reg: %d value: 0x%04x\n", reg_num, val);

    switch (reg_num) {
    case PHY_REG_CONTROL:
        if (val & PHY_REG_CONTROL_RST) {
            /* Phy reset */
            gem_phy_reset(s);
            val &= ~(PHY_REG_CONTROL_RST | PHY_REG_CONTROL_LOOP);
            s->phy_loop = 0;
        }
        if (val & PHY_REG_CONTROL_ANEG) {
            /* Complete autonegotiation immediately */
            val &= ~(PHY_REG_CONTROL_ANEG | PHY_REG_CONTROL_ANRESTART);
            s->phy_regs[PHY_REG_STATUS] |= PHY_REG_STATUS_ANEGCMPL;
        }
        if (val & PHY_REG_CONTROL_LOOP) {
            DB_PRINT("PHY placed in loopback\n");
            s->phy_loop = 1;
        } else {
            s->phy_loop = 0;
        }
        break;
    }
    s->phy_regs[reg_num] = val;
}