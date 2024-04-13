static inline unsigned tx_desc_get_length(uint32_t *desc)
{
    return desc[1] & DESC_1_LENGTH;
}