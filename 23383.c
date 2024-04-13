static inline void rx_desc_set_unicast_hash(uint32_t *desc)
{
    desc[1] |= R_DESC_1_RX_UNICAST_HASH;
}