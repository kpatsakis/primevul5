USBDevice *usb_host_device_open(const char *devname)
{
    Monitor *mon = cur_mon;
    int bus_num, addr;
    char product_name[PRODUCT_NAME_SZ];

    if (strstr(devname, "auto:")) {
        usb_host_auto_add(devname);
        return NULL;
    }

    if (usb_host_find_device(&bus_num, &addr, product_name, sizeof(product_name),
                             devname) < 0)
        return NULL;

    if (hostdev_find(bus_num, addr)) {
       monitor_printf(mon, "husb: host usb device %d.%d is already open\n",
                      bus_num, addr);
       return NULL;
    }

    return usb_host_device_open_addr(bus_num, addr, product_name);
}