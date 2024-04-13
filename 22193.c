static uint32_t sungem_mii_op(SunGEMState *s, uint32_t val)
{
    uint8_t phy_addr, reg_addr, op;

    /* Ignore not start of frame */
    if ((val >> 30) != 1) {
        trace_sungem_mii_invalid_sof(val >> 30);
        return 0xffff;
    }
    phy_addr = (val & MIF_FRAME_PHYAD) >> 23;
    reg_addr = (val & MIF_FRAME_REGAD) >> 18;
    op = (val & MIF_FRAME_OP) >> 28;
    switch (op) {
    case 1:
        sungem_mii_write(s, phy_addr, reg_addr, val & MIF_FRAME_DATA);
        return val | MIF_FRAME_TALSB;
    case 2:
        return sungem_mii_read(s, phy_addr, reg_addr) | MIF_FRAME_TALSB;
    default:
        trace_sungem_mii_invalid_op(op);
    }
    return 0xffff | MIF_FRAME_TALSB;
}