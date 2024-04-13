static void sungem_realize(PCIDevice *pci_dev, Error **errp)
{
    DeviceState *dev = DEVICE(pci_dev);
    SunGEMState *s = SUNGEM(pci_dev);
    uint8_t *pci_conf;

    pci_conf = pci_dev->config;

    pci_set_word(pci_conf + PCI_STATUS,
                 PCI_STATUS_FAST_BACK |
                 PCI_STATUS_DEVSEL_MEDIUM |
                 PCI_STATUS_66MHZ);

    pci_set_word(pci_conf + PCI_SUBSYSTEM_VENDOR_ID, 0x0);
    pci_set_word(pci_conf + PCI_SUBSYSTEM_ID, 0x0);

    pci_conf[PCI_INTERRUPT_PIN] = 1; /* interrupt pin A */
    pci_conf[PCI_MIN_GNT] = 0x40;
    pci_conf[PCI_MAX_LAT] = 0x40;

    sungem_reset_all(s, true);
    memory_region_init(&s->sungem, OBJECT(s), "sungem", SUNGEM_MMIO_SIZE);

    memory_region_init_io(&s->greg, OBJECT(s), &sungem_mmio_greg_ops, s,
                          "sungem.greg", SUNGEM_MMIO_GREG_SIZE);
    memory_region_add_subregion(&s->sungem, 0, &s->greg);

    memory_region_init_io(&s->txdma, OBJECT(s), &sungem_mmio_txdma_ops, s,
                          "sungem.txdma", SUNGEM_MMIO_TXDMA_SIZE);
    memory_region_add_subregion(&s->sungem, 0x2000, &s->txdma);

    memory_region_init_io(&s->rxdma, OBJECT(s), &sungem_mmio_rxdma_ops, s,
                          "sungem.rxdma", SUNGEM_MMIO_RXDMA_SIZE);
    memory_region_add_subregion(&s->sungem, 0x4000, &s->rxdma);

    memory_region_init_io(&s->mac, OBJECT(s), &sungem_mmio_mac_ops, s,
                          "sungem.mac", SUNGEM_MMIO_MAC_SIZE);
    memory_region_add_subregion(&s->sungem, 0x6000, &s->mac);

    memory_region_init_io(&s->mif, OBJECT(s), &sungem_mmio_mif_ops, s,
                          "sungem.mif", SUNGEM_MMIO_MIF_SIZE);
    memory_region_add_subregion(&s->sungem, 0x6200, &s->mif);

    memory_region_init_io(&s->pcs, OBJECT(s), &sungem_mmio_pcs_ops, s,
                          "sungem.pcs", SUNGEM_MMIO_PCS_SIZE);
    memory_region_add_subregion(&s->sungem, 0x9000, &s->pcs);

    pci_register_bar(pci_dev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->sungem);

    qemu_macaddr_default_if_unset(&s->conf.macaddr);
    s->nic = qemu_new_nic(&net_sungem_info, &s->conf,
                          object_get_typename(OBJECT(dev)),
                          dev->id, s);
    qemu_format_nic_info_str(qemu_get_queue(s->nic),
                             s->conf.macaddr.a);
}