static void vdi_port_read_buf_unref(VDIReadBuf *buf)
{
    if (!--buf->refs) {
        ring_add(&reds->agent_state.read_bufs, &buf->link);

        /* read_one_msg_from_vdi_port may have never completed because the read_bufs
        ring was empty. So we call it again so it can complete its work if
        necessary. Note that since we can be called from spice_char_device_wakeup
        this can cause recursion, but we have protection for that */
        if (reds->agent_state.base) {
            spice_char_device_wakeup(reds->agent_state.base);
        }
    }
}
