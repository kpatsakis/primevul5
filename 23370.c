static inline int gem_get_desc_len(CadenceGEMState *s, bool rx_n_tx)
{
    int ret = 2;

    if (s->regs[GEM_DMACFG] & GEM_DMACFG_ADDR_64B) {
        ret += 2;
    }
    if (s->regs[GEM_DMACFG] & (rx_n_tx ? GEM_DMACFG_RX_BD_EXT
                                       : GEM_DMACFG_TX_BD_EXT)) {
        ret += 2;
    }

    assert(ret <= DESC_MAX_NUM_WORDS);
    return ret;
}