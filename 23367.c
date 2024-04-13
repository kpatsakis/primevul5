static inline uint32_t gem_get_rx_queue_base_addr(CadenceGEMState *s, int q)
{
    return gem_get_queue_base_addr(s, false, q);
}