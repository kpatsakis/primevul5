static VDIReadBuf* vdi_port_read_buf_ref(VDIReadBuf *buf)
{
    buf->refs++;
    return buf;
}
