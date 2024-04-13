static void sm501_realize_pci(PCIDevice *dev, Error **errp)
{
    SM501PCIState *s = PCI_SM501(dev);

    sm501_init(&s->state, DEVICE(dev), s->vram_size);
    if (get_local_mem_size(&s->state) != s->vram_size) {
        error_setg(errp, "Invalid VRAM size, nearest valid size is %" PRIu32,
                   get_local_mem_size(&s->state));
        return;
    }
    pci_register_bar(dev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY,
                     &s->state.local_mem_region);
    pci_register_bar(dev, 1, PCI_BASE_ADDRESS_SPACE_MEMORY,
                     &s->state.mmio_region);
}