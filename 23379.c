static inline void rx_desc_set_sof(uint32_t *desc)
{
    desc[1] |= DESC_1_RX_SOF;
}