static void gem_set_isr(CadenceGEMState *s, int q, uint32_t flag)
{
    if (q == 0) {
        s->regs[GEM_ISR] |= flag & ~(s->regs[GEM_IMR]);
    } else {
        s->regs[GEM_INT_Q1_STATUS + q - 1] |= flag &
                                      ~(s->regs[GEM_INT_Q1_MASK + q - 1]);
    }
}