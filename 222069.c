static int usb_host_info_device(void *opaque, int bus_num, int addr,
                                int class_id,
                                int vendor_id, int product_id,
                                const char *product_name,
                                int speed)
{
    usb_info_device(bus_num, addr, class_id, vendor_id, product_id,
                    product_name, speed);
    return 0;
}