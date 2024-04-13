static void sungem_eval_irq(SunGEMState *s)
{
    uint32_t stat, mask;

    mask = s->gregs[GREG_IMASK >> 2];
    stat = s->gregs[GREG_STAT >> 2] & ~GREG_STAT_TXNR;
    if (stat & ~mask) {
        pci_set_irq(PCI_DEVICE(s), 1);
    } else {
        pci_set_irq(PCI_DEVICE(s), 0);
    }
}