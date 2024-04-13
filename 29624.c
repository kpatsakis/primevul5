static VDIReadBuf *vdi_port_read_buf_get(void)
{
    VDIPortState *state = &reds->agent_state;
    RingItem *item;
    VDIReadBuf *buf;

    if (!(item = ring_get_head(&state->read_bufs))) {
        return NULL;
    }

    ring_remove(item);
    buf = SPICE_CONTAINEROF(item, VDIReadBuf, link);

    buf->refs = 1;
    return buf;
}
