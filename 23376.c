static void gem_update_int_status(CadenceGEMState *s)
{
    int i;

    qemu_set_irq(s->irq[0], !!s->regs[GEM_ISR]);

    for (i = 1; i < s->num_priority_queues; ++i) {
        qemu_set_irq(s->irq[i], !!s->regs[GEM_INT_Q1_STATUS + i - 1]);
    }
}