static hwaddr gem_get_rx_desc_addr(CadenceGEMState *s, int q)
{
    return gem_get_desc_addr(s, false, q);
}