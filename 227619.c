static void msf2_emac_do_reset(MSF2EmacState *s)
{
    memset(&s->regs[0], 0, sizeof(s->regs));
    s->regs[R_CFG1] = 0x80000000;
    s->regs[R_CFG2] = 0x00007000;
    s->regs[R_IFG] = 0x40605060;
    s->regs[R_HALF_DUPLEX] = 0x00A1F037;
    s->regs[R_MAX_FRAME_LENGTH] = 0x00000600;
    s->regs[R_FIFO_CFG5] = 0X3FFFF;

    msf2_phy_reset(s);
}