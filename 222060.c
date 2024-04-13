static int do_token_out(USBDevice *dev, USBPacket *p)
{
    USBHostDevice *s = (USBHostDevice *) dev;

    if (p->devep != 0)
        return usb_host_handle_data(s, p);

    switch(s->ctrl.state) {
    case CTRL_STATE_ACK:
        if (s->ctrl.req.bRequestType & USB_DIR_IN) {
            s->ctrl.state = CTRL_STATE_IDLE;
            /* transfer OK */
        } else {
            /* ignore additional output */
        }
        return 0;

    case CTRL_STATE_DATA:
        if (!(s->ctrl.req.bRequestType & USB_DIR_IN)) {
            int len = s->ctrl.len - s->ctrl.offset;
            if (len > p->len)
                len = p->len;
            memcpy(s->ctrl.buffer + s->ctrl.offset, p->data, len);
            s->ctrl.offset += len;
            if (s->ctrl.offset >= s->ctrl.len)
                s->ctrl.state = CTRL_STATE_ACK;
            return len;
        }

        s->ctrl.state = CTRL_STATE_IDLE;
        return USB_RET_STALL;

    default:
        return USB_RET_STALL;
    }
}