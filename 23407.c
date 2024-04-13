static inline void rx_desc_set_ownership(uint32_t *desc)
{
    desc[0] |= DESC_0_RX_OWNERSHIP;
}