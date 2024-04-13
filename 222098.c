static int usb_host_find_device(int *pbus_num, int *paddr,
                                char *product_name, int product_name_size,
                                const char *devname)
{
    const char *p;
    int ret;
    FindDeviceState fs;

    p = strchr(devname, '.');
    if (p) {
        *pbus_num = strtoul(devname, NULL, 0);
        *paddr = strtoul(p + 1, NULL, 0);
        fs.bus_num = *pbus_num;
        fs.addr = *paddr;
        ret = usb_host_scan(&fs, usb_host_find_device_scan);
        if (ret)
            pstrcpy(product_name, product_name_size, fs.product_name);
        return 0;
    }

    p = strchr(devname, ':');
    if (p) {
        fs.vendor_id = strtoul(devname, NULL, 16);
        fs.product_id = strtoul(p + 1, NULL, 16);
        ret = usb_host_scan(&fs, usb_host_find_device_scan);
        if (ret) {
            *pbus_num = fs.bus_num;
            *paddr = fs.addr;
            pstrcpy(product_name, product_name_size, fs.product_name);
            return 0;
        }
    }
    return -1;
}