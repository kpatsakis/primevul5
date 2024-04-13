int usb_host_device_close(const char *devname)
{
    char product_name[PRODUCT_NAME_SZ];
    int bus_num, addr;
    USBHostDevice *s;

    if (strstr(devname, "auto:"))
        return usb_host_auto_del(devname);

    if (usb_host_find_device(&bus_num, &addr, product_name, sizeof(product_name),
                             devname) < 0)
        return -1;
 
    s = hostdev_find(bus_num, addr);
    if (s) {
        usb_device_del_addr(0, s->dev.addr);
        return 0;
    }

    return -1;
}