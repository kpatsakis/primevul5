static inline uint64_t tx_desc_get_buffer(CadenceGEMState *s, uint32_t *desc)
{
    uint64_t ret = desc[0];

    if (s->regs[GEM_DMACFG] & GEM_DMACFG_ADDR_64B) {
        ret |= (uint64_t)desc[2] << 32;
    }
    return ret;
}