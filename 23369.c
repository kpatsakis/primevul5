static void phy_update_link(CadenceGEMState *s)
{
    DB_PRINT("down %d\n", qemu_get_queue(s->nic)->link_down);

    /* Autonegotiation status mirrors link status.  */
    if (qemu_get_queue(s->nic)->link_down) {
        s->phy_regs[PHY_REG_STATUS] &= ~(PHY_REG_STATUS_ANEGCMPL |
                                         PHY_REG_STATUS_LINK);
        s->phy_regs[PHY_REG_INT_ST] |= PHY_REG_INT_ST_LINKC;
    } else {
        s->phy_regs[PHY_REG_STATUS] |= (PHY_REG_STATUS_ANEGCMPL |
                                         PHY_REG_STATUS_LINK);
        s->phy_regs[PHY_REG_INT_ST] |= (PHY_REG_INT_ST_LINKC |
                                        PHY_REG_INT_ST_ANEGCMPL |
                                        PHY_REG_INT_ST_ENERGY);
    }
}