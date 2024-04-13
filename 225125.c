static void pci_piix_ide_realize(PCIDevice *dev, Error **errp)
{
    PCIIDEState *d = PCI_IDE(dev);
    uint8_t *pci_conf = dev->config;

    pci_conf[PCI_CLASS_PROG] = 0x80; // legacy ATA mode

    qemu_register_reset(piix3_reset, d);

    bmdma_setup_bar(d);
    pci_register_bar(dev, 4, PCI_BASE_ADDRESS_SPACE_IO, &d->bmdma_bar);

    vmstate_register(DEVICE(dev), 0, &vmstate_ide_pci, d);

    pci_piix_init_ports(d);
}