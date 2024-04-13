static uint16_t gem_phy_read(CadenceGEMState *s, unsigned reg_num)
{
    DB_PRINT("reg: %d value: 0x%04x\n", reg_num, s->phy_regs[reg_num]);
    return s->phy_regs[reg_num];
}