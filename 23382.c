static inline void rx_desc_set_multicast_hash(uint32_t *desc)
{
    desc[1] |= R_DESC_1_RX_MULTICAST_HASH;
}