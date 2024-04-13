static void vdi_port_read_buf_release(uint8_t *data, void *opaque)
{
    VDIReadBuf *buf = (VDIReadBuf *)opaque;

    vdi_port_read_buf_unref(buf);
}
