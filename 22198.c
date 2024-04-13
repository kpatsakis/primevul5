static uint16_t sungem_mii_read(SunGEMState *s, uint8_t phy_addr,
                                uint8_t reg_addr)
{
    uint16_t val;

    val = __sungem_mii_read(s, phy_addr, reg_addr);

    trace_sungem_mii_read(phy_addr, reg_addr, val);

    return val;
}