static int do_token_setup(USBDevice *dev, USBPacket *p)
{
    USBHostDevice *s = (USBHostDevice *) dev;
    int ret = 0;

    if (p->len != 8)
        return USB_RET_STALL;
 
    memcpy(&s->ctrl.req, p->data, 8);
    s->ctrl.len    = le16_to_cpu(s->ctrl.req.wLength);
    s->ctrl.offset = 0;
    s->ctrl.state  = CTRL_STATE_SETUP;

    if (s->ctrl.req.bRequestType & USB_DIR_IN) {
        ret = usb_host_handle_control(s, p);
        if (ret < 0)
            return ret;

        if (ret < s->ctrl.len)
            s->ctrl.len = ret;
        s->ctrl.state = CTRL_STATE_DATA;
    } else {
        if (s->ctrl.len == 0)
            s->ctrl.state = CTRL_STATE_ACK;
        else
            s->ctrl.state = CTRL_STATE_DATA;
    }

    return ret;
}