static inline void rx_desc_clear_control(uint32_t *desc)
{
    desc[1]  = 0;
}