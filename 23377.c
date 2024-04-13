static inline unsigned tx_desc_get_wrap(uint32_t *desc)
{
    return (desc[1] & DESC_1_TX_WRAP) ? 1 : 0;
}