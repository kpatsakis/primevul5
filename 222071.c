void usb_host_info(Monitor *mon)
{
    struct USBAutoFilter *f;

    usb_host_scan(NULL, usb_host_info_device);

    if (usb_auto_filter)
        monitor_printf(mon, "  Auto filters:\n");
    for (f = usb_auto_filter; f; f = f->next) {
        char bus[10], addr[10], vid[10], pid[10];
        dec2str(f->bus_num, bus, sizeof(bus));
        dec2str(f->addr, addr, sizeof(addr));
        hex2str(f->vendor_id, vid, sizeof(vid));
        hex2str(f->product_id, pid, sizeof(pid));
        monitor_printf(mon, "    Device %s.%s ID %s:%s\n",
                       bus, addr, vid, pid);
    }
}