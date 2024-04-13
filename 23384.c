static inline unsigned tx_desc_get_used(uint32_t *desc)
{
    return (desc[1] & DESC_1_USED) ? 1 : 0;
}