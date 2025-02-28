static void vnc_client_write_locked(void *opaque)
{
    VncState *vs = opaque;

#ifdef CONFIG_VNC_SASL
    if (vs->sasl.conn &&
        vs->sasl.runSSF &&
        !vs->sasl.waitWriteSSF) {
        vnc_client_write_sasl(vs);
    } else
#endif /* CONFIG_VNC_SASL */
    {
        if (vs->encode_ws) {
            vnc_client_write_ws(vs);
        } else {
            vnc_client_write_plain(vs);
        }
    }
}