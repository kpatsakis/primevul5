static void sungem_reset_rx(SunGEMState *s)
{
    trace_sungem_rx_reset();

    /* XXX Do RXCFG */
    /* XXX Check value */
    s->rxdmaregs[RXDMA_FSZ >> 2] = 0x140;
    s->rxdmaregs[RXDMA_DONE >> 2] = 0;
    s->rxdmaregs[RXDMA_KICK >> 2] = 0;
    s->rxdmaregs[RXDMA_CFG >> 2] = 0x1000010;
    s->rxdmaregs[RXDMA_PTHRESH >> 2] = 0xf8;
    s->rxdmaregs[RXDMA_BLANK >> 2] = 0;

    sungem_update_masks(s);
}