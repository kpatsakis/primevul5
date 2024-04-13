static void sm501_reset_pci(DeviceState *dev)
{
    SM501PCIState *s = PCI_SM501(dev);
    sm501_reset(&s->state);
    /* Bits 2:0 of misc_control register is 001 for PCI */
    s->state.misc_control |= 1;
}