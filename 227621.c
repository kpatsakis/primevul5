static bool emac_can_rx(NetClientState *nc)
{
    MSF2EmacState *s = qemu_get_nic_opaque(nc);

    return (s->regs[R_CFG1] & R_CFG1_RX_EN_MASK) &&
           (s->regs[R_DMA_RX_CTL] & R_DMA_RX_CTL_EN_MASK);
}