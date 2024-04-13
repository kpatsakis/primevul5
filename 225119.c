PCIDevice *pci_piix3_xen_ide_init(PCIBus *bus, DriveInfo **hd_table, int devfn)
{
    PCIDevice *dev;

    dev = pci_create_simple(bus, devfn, "piix3-ide-xen");
    pci_ide_create_devs(dev, hd_table);
    return dev;
}