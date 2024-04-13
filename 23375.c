static inline void tx_desc_set_used(uint32_t *desc)
{
    desc[1] |= DESC_1_USED;
}