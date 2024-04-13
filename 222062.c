static int usb_host_find_device_scan(void *opaque, int bus_num, int addr,
                                     int class_id,
                                     int vendor_id, int product_id,
                                     const char *product_name, int speed)
{
    FindDeviceState *s = opaque;
    if ((vendor_id == s->vendor_id &&
        product_id == s->product_id) ||
        (bus_num == s->bus_num &&
        addr == s->addr)) {
        pstrcpy(s->product_name, PRODUCT_NAME_SZ, product_name);
        s->bus_num = bus_num;
        s->addr = addr;
        return 1;
    } else {
        return 0;
    }
}