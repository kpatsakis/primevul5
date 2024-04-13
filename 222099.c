static void async_complete_ctrl(USBHostDevice *s, USBPacket *p)
{
    switch(s->ctrl.state) {
    case CTRL_STATE_SETUP:
        if (p->len < s->ctrl.len)
            s->ctrl.len = p->len;
        s->ctrl.state = CTRL_STATE_DATA;
        p->len = 8;
        break;

    case CTRL_STATE_ACK:
        s->ctrl.state = CTRL_STATE_IDLE;
        p->len = 0;
        break;

    default:
        break;
    }
}