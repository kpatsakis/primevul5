static int usb_host_auto_del(const char *spec)
{
    struct USBAutoFilter *pf = usb_auto_filter;
    struct USBAutoFilter **prev = &usb_auto_filter;
    struct USBAutoFilter filter;

    if (parse_filter(spec, &filter) < 0)
        return -1;

    while (pf) {
        if (match_filter(pf, &filter)) {
            dprintf("husb: removed auto filter: bus_num %d addr %d vid %d pid %d\n",
	             pf->bus_num, pf->addr, pf->vendor_id, pf->product_id);

            *prev = pf->next;

	    if (!usb_auto_filter) {
                /* No more filters. Stop scanning. */
                qemu_del_timer(usb_auto_timer);
                qemu_free_timer(usb_auto_timer);
            }

            return 0;
        }

        prev = &pf->next;
        pf   = pf->next;
    }

    return -1;
}