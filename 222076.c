static void usb_host_handle_reset(USBDevice *dev)
{
    USBHostDevice *s = (USBHostDevice *) dev;

    dprintf("husb: reset device %u.%u\n", s->bus_num, s->addr);

    ioctl(s->fd, USBDEVFS_RESET);

    usb_host_claim_interfaces(s, s->configuration);
}