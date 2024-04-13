static int usb_host_set_address(USBHostDevice *s, int addr)
{
    dprintf("husb: ctrl set addr %u\n", addr);
    s->dev.addr = addr;
    return 0;
}