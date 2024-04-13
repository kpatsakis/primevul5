static void sungem_reset_tx(SunGEMState *s)
{
    trace_sungem_tx_reset();

    /* XXX Do TXCFG */
    /* XXX Check value */
    s->txdmaregs[TXDMA_FSZ >> 2] = 0x90;
    s->txdmaregs[TXDMA_TXDONE >> 2] = 0;
    s->txdmaregs[TXDMA_KICK >> 2] = 0;
    s->txdmaregs[TXDMA_CFG >> 2] = 0x118010;

    sungem_update_masks(s);

    s->tx_size = 0;
    s->tx_first_ctl = 0;
}