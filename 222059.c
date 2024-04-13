static int usb_host_auto_add(const char *spec)
{
    struct USBAutoFilter filter, *f;

    if (parse_filter(spec, &filter) < 0)
        return -1;

    f = qemu_mallocz(sizeof(*f));

    *f = filter; 

    if (!usb_auto_filter) {
        /*
         * First entry. Init and start the monitor.
         * Right now we're using timer to check for new devices.
         * If this turns out to be too expensive we can move that into a 
         * separate thread.
         */
	usb_auto_timer = qemu_new_timer(rt_clock, usb_host_auto_timer, NULL);
	if (!usb_auto_timer) {
            fprintf(stderr, "husb: failed to allocate auto scan timer\n");
            qemu_free(f);
            return -1;
        }

        /* Check for new devices every two seconds */
        qemu_mod_timer(usb_auto_timer, qemu_get_clock(rt_clock) + 2000);
    }

    dprintf("husb: added auto filter: bus_num %d addr %d vid %d pid %d\n",
	f->bus_num, f->addr, f->vendor_id, f->product_id);

    f->next = usb_auto_filter;
    usb_auto_filter = f;

    return 0;
}