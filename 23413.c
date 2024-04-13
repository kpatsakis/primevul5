static inline unsigned rx_desc_get_wrap(uint32_t *desc)
{
    return desc[0] & DESC_0_RX_WRAP ? 1 : 0;
}