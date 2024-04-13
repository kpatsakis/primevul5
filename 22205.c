static void sungem_update_status(SunGEMState *s, uint32_t bits, bool val)
{
    uint32_t stat;

    stat = s->gregs[GREG_STAT >> 2];
    if (val) {
        stat |= bits;
    } else {
        stat &= ~bits;
    }
    s->gregs[GREG_STAT >> 2] = stat;
    sungem_eval_irq(s);
}