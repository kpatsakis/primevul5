static inline void rx_desc_set_length(uint32_t *desc, unsigned len)
{
    desc[1] &= ~DESC_1_LENGTH;
    desc[1] |= len;
}