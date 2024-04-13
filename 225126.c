static void piix3_reset(void *opaque)
{
    PCIIDEState *d = opaque;
    PCIDevice *pd = PCI_DEVICE(d);
    uint8_t *pci_conf = pd->config;
    int i;

    for (i = 0; i < 2; i++) {
        ide_bus_reset(&d->bus[i]);
    }

    /* TODO: this is the default. do not override. */
    pci_conf[PCI_COMMAND] = 0x00;
    /* TODO: this is the default. do not override. */
    pci_conf[PCI_COMMAND + 1] = 0x00;
    /* TODO: use pci_set_word */
    pci_conf[PCI_STATUS] = PCI_STATUS_FAST_BACK;
    pci_conf[PCI_STATUS + 1] = PCI_STATUS_DEVSEL_MEDIUM >> 8;
    pci_conf[0x20] = 0x01; /* BMIBA: 20-23h */
}