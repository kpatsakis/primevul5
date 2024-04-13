static hwaddr gem_get_tx_desc_addr(CadenceGEMState *s, int q)
{
    return gem_get_desc_addr(s, true, q);
}