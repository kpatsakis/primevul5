static void usb_host_auto_timer(void *unused)
{
    usb_host_scan(NULL, usb_host_auto_scan);
    qemu_mod_timer(usb_auto_timer, qemu_get_clock(rt_clock) + 2000);
}