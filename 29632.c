static void vdi_port_unref_msg_to_client(SpiceCharDeviceMsgToClient *msg,
                                  void *opaque)
{
    vdi_port_read_buf_unref(msg);
}
