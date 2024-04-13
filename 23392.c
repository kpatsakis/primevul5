static inline unsigned tx_desc_get_last(uint32_t *desc)
{
    return (desc[1] & DESC_1_TX_LAST) ? 1 : 0;
}