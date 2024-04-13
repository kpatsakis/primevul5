static void usb_info_device(int bus_num, int addr, int class_id,
                            int vendor_id, int product_id,
                            const char *product_name,
                            int speed)
{
    Monitor *mon = cur_mon;
    const char *class_str, *speed_str;

    switch(speed) {
    case USB_SPEED_LOW:
        speed_str = "1.5";
        break;
    case USB_SPEED_FULL:
        speed_str = "12";
        break;
    case USB_SPEED_HIGH:
        speed_str = "480";
        break;
    default:
        speed_str = "?";
        break;
    }

    monitor_printf(mon, "  Device %d.%d, speed %s Mb/s\n",
                bus_num, addr, speed_str);
    class_str = usb_class_str(class_id);
    if (class_str)
        monitor_printf(mon, "    %s:", class_str);
    else
        monitor_printf(mon, "    Class %02x:", class_id);
    monitor_printf(mon, " USB device %04x:%04x", vendor_id, product_id);
    if (product_name[0] != '\0')
        monitor_printf(mon, ", %s", product_name);
    monitor_printf(mon, "\n");
}