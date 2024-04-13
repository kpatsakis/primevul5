static hwaddr gem_get_desc_addr(CadenceGEMState *s, bool tx, int q)
{
    hwaddr desc_addr = 0;

    if (s->regs[GEM_DMACFG] & GEM_DMACFG_ADDR_64B) {
        desc_addr = s->regs[tx ? GEM_TBQPH : GEM_RBQPH];
    }
    desc_addr <<= 32;
    desc_addr |= tx ? s->tx_desc_addr[q] : s->rx_desc_addr[q];
    return desc_addr;
}