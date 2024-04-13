static inline unsigned rx_desc_get_ownership(uint32_t *desc)
{
    return desc[0] & DESC_0_RX_OWNERSHIP ? 1 : 0;
}