static void async_complete(void *opaque)
{
    USBHostDevice *s = opaque;
    AsyncURB *aurb;

    while (1) {
    	USBPacket *p;

	int r = ioctl(s->fd, USBDEVFS_REAPURBNDELAY, &aurb);
        if (r < 0) {
            if (errno == EAGAIN)
                return;

            if (errno == ENODEV && !s->closing) {
                printf("husb: device %d.%d disconnected\n", s->bus_num, s->addr);
	        usb_device_del_addr(0, s->dev.addr);
                return;
            }

            dprintf("husb: async. reap urb failed errno %d\n", errno);
            return;
        }

        p = aurb->packet;

	dprintf("husb: async completed. aurb %p status %d alen %d\n", 
                aurb, aurb->urb.status, aurb->urb.actual_length);

	if (p) {
            switch (aurb->urb.status) {
            case 0:
                p->len = aurb->urb.actual_length;
                if (aurb->urb.type == USBDEVFS_URB_TYPE_CONTROL)
                    async_complete_ctrl(s, p);
                break;

            case -EPIPE:
                set_halt(s, p->devep);
                /* fall through */
            default:
                p->len = USB_RET_NAK;
                break;
            }

            usb_packet_complete(p);
	}

        async_free(aurb);
    }
}