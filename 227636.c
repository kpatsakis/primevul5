static void emac_update_irq(MSF2EmacState *s)
{
    bool intr = emac_get_isr(s);

    qemu_set_irq(s->irq, intr);
}