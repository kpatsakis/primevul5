static uint16_t __sungem_mii_read(SunGEMState *s, uint8_t phy_addr,
                                  uint8_t reg_addr)
{
    if (phy_addr != s->phy_addr) {
        return 0xffff;
    }
    /* Primitive emulation of a BCM5201 to please the driver,
     * ID is 0x00406210. TODO: Do a gigabit PHY like BCM5400
     */
    switch (reg_addr) {
    case MII_BMCR:
        return 0;
    case MII_PHYID1:
        return 0x0040;
    case MII_PHYID2:
        return 0x6210;
    case MII_BMSR:
        if (qemu_get_queue(s->nic)->link_down) {
            return MII_BMSR_100TX_FD  | MII_BMSR_AUTONEG;
        } else {
            return MII_BMSR_100TX_FD | MII_BMSR_AN_COMP |
                    MII_BMSR_AUTONEG | MII_BMSR_LINK_ST;
        }
    case MII_ANLPAR:
    case MII_ANAR:
        return MII_ANLPAR_TXFD;
    case 0x18: /* 5201 AUX status */
        return 3; /* 100FD */
    default:
        return 0;
    };
}