static void sm501_pci_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->realize = sm501_realize_pci;
    k->vendor_id = PCI_VENDOR_ID_SILICON_MOTION;
    k->device_id = PCI_DEVICE_ID_SM501;
    k->class_id = PCI_CLASS_DISPLAY_OTHER;
    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);
    dc->desc = "SM501 Display Controller";
    device_class_set_props(dc, sm501_pci_properties);
    dc->reset = sm501_reset_pci;
    dc->hotpluggable = false;
    dc->vmsd = &vmstate_sm501_pci;
}