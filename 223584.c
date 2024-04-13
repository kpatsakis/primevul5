static void sm501_realize_sysbus(DeviceState *dev, Error **errp)
{
    SM501SysBusState *s = SYSBUS_SM501(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
    DeviceState *usb_dev;
    MemoryRegion *mr;

    sm501_init(&s->state, dev, s->vram_size);
    if (get_local_mem_size(&s->state) != s->vram_size) {
        error_setg(errp, "Invalid VRAM size, nearest valid size is %" PRIu32,
                   get_local_mem_size(&s->state));
        return;
    }
    sysbus_init_mmio(sbd, &s->state.local_mem_region);
    sysbus_init_mmio(sbd, &s->state.mmio_region);

    /* bridge to usb host emulation module */
    usb_dev = qdev_create(NULL, "sysbus-ohci");
    qdev_prop_set_uint32(usb_dev, "num-ports", 2);
    qdev_prop_set_uint64(usb_dev, "dma-offset", s->base);
    qdev_init_nofail(usb_dev);
    memory_region_add_subregion(&s->state.mmio_region, SM501_USB_HOST,
                       sysbus_mmio_get_region(SYS_BUS_DEVICE(usb_dev), 0));
    sysbus_pass_irq(sbd, SYS_BUS_DEVICE(usb_dev));

    /* bridge to serial emulation module */
    qdev_init_nofail(DEVICE(&s->serial));
    mr = sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->serial), 0);
    memory_region_add_subregion(&s->state.mmio_region, SM501_UART0, mr);
    /* TODO : chain irq to IRL */
}