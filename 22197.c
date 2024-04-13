static void sungem_update_masks(SunGEMState *s)
{
    uint32_t sz;

    sz = 1 << (((s->rxdmaregs[RXDMA_CFG >> 2] & RXDMA_CFG_RINGSZ) >> 1) + 5);
    s->rx_mask = sz - 1;

    sz = 1 << (((s->txdmaregs[TXDMA_CFG >> 2] & TXDMA_CFG_RINGSZ) >> 1) + 5);
    s->tx_mask = sz - 1;
}