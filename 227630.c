static uint32_t emac_get_isr(MSF2EmacState *s)
{
    uint32_t ier = s->regs[R_DMA_IRQ_MASK];
    uint32_t tx = s->regs[R_DMA_TX_STATUS] & 0xF;
    uint32_t rx = s->regs[R_DMA_RX_STATUS] & 0xF;
    uint32_t isr = (rx << 4) | tx;

    s->regs[R_DMA_IRQ] = ier & isr;
    return s->regs[R_DMA_IRQ];
}