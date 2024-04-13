static uint32_t gem_get_max_buf_len(CadenceGEMState *s, bool tx)
{
    uint32_t size;
    if (s->regs[GEM_NWCFG] & GEM_NWCFG_JUMBO_FRAME) {
        size = s->regs[GEM_JUMBO_MAX_LEN];
        if (size > s->jumbo_max_len) {
            size = s->jumbo_max_len;
            qemu_log_mask(LOG_GUEST_ERROR, "GEM_JUMBO_MAX_LEN reg cannot be"
                " greater than 0x%" PRIx32 "\n", s->jumbo_max_len);
        }
    } else if (tx) {
        size = 1518;
    } else {
        size = s->regs[GEM_NWCFG] & GEM_NWCFG_RCV_1538 ? 1538 : 1518;
    }
    return size;
}