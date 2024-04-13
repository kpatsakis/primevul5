static void pci_piix_init_ports(PCIIDEState *d) {
    static const struct {
        int iobase;
        int iobase2;
        int isairq;
    } port_info[] = {
        {0x1f0, 0x3f6, 14},
        {0x170, 0x376, 15},
    };
    int i;

    for (i = 0; i < 2; i++) {
        ide_bus_new(&d->bus[i], sizeof(d->bus[i]), DEVICE(d), i, 2);
        ide_init_ioport(&d->bus[i], NULL, port_info[i].iobase,
                        port_info[i].iobase2);
        ide_init2(&d->bus[i], isa_get_irq(NULL, port_info[i].isairq));

        bmdma_init(&d->bus[i], &d->bmdma[i], d);
        d->bmdma[i].bus = &d->bus[i];
        ide_register_restart_cb(&d->bus[i]);
    }
}