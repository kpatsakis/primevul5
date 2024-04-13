static int usb_host_handle_data(USBHostDevice *s, USBPacket *p)
{
    struct usbdevfs_urb *urb;
    AsyncURB *aurb;
    int ret;

    aurb = async_alloc();
    aurb->hdev   = s;
    aurb->packet = p;

    urb = &aurb->urb;

    if (p->pid == USB_TOKEN_IN)
    	urb->endpoint = p->devep | 0x80;
    else
    	urb->endpoint = p->devep;

    if (is_halted(s, p->devep)) {
	ret = ioctl(s->fd, USBDEVFS_CLEAR_HALT, &urb->endpoint);
        if (ret < 0) {
            dprintf("husb: failed to clear halt. ep 0x%x errno %d\n", 
                   urb->endpoint, errno);
            return USB_RET_NAK;
        }
        clear_halt(s, p->devep);
    }

    urb->buffer        = p->data;
    urb->buffer_length = p->len;

    if (is_isoc(s, p->devep)) {
        /* Setup ISOC transfer */
        urb->type     = USBDEVFS_URB_TYPE_ISO;
        urb->flags    = USBDEVFS_URB_ISO_ASAP;
        urb->number_of_packets = 1;
        urb->iso_frame_desc[0].length = p->len;
    } else {
        /* Setup bulk transfer */
        urb->type     = USBDEVFS_URB_TYPE_BULK;
    }

    urb->usercontext = s;

    ret = ioctl(s->fd, USBDEVFS_SUBMITURB, urb);

    dprintf("husb: data submit. ep 0x%x len %u aurb %p\n", urb->endpoint, p->len, aurb);

    if (ret < 0) {
        dprintf("husb: submit failed. errno %d\n", errno);
        async_free(aurb);

        switch(errno) {
        case ETIMEDOUT:
            return USB_RET_NAK;
        case EPIPE:
        default:
            return USB_RET_STALL;
        }
    }

    usb_defer_packet(p, async_cancel, aurb);
    return USB_RET_ASYNC;
}