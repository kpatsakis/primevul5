static inline void rx_desc_set_eof(uint32_t *desc)
{
    desc[1] |= DESC_1_RX_EOF;
}