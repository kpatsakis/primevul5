static int usb_host_handle_packet(USBDevice *s, USBPacket *p)
{
    switch(p->pid) {
    case USB_MSG_ATTACH:
        s->state = USB_STATE_ATTACHED;
        return 0;

    case USB_MSG_DETACH:
        s->state = USB_STATE_NOTATTACHED;
        return 0;

    case USB_MSG_RESET:
        s->remote_wakeup = 0;
        s->addr = 0;
        s->state = USB_STATE_DEFAULT;
        s->handle_reset(s);
        return 0;
    }

    /* Rest of the PIDs must match our address */
    if (s->state < USB_STATE_DEFAULT || p->devaddr != s->addr)
        return USB_RET_NODEV;

    switch (p->pid) {
    case USB_TOKEN_SETUP:
        return do_token_setup(s, p);

    case USB_TOKEN_IN:
        return do_token_in(s, p);

    case USB_TOKEN_OUT:
        return do_token_out(s, p);
 
    default:
        return USB_RET_STALL;
    }
}