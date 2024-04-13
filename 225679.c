capture_tcp(const guchar *pd, int offset, int len, capture_packet_info_t *cpinfo, const union wtap_pseudo_header *pseudo_header)
{
    guint16 src_port, dst_port, low_port, high_port;

    if (!BYTES_ARE_IN_FRAME(offset, len, 4))
        return FALSE;

    capture_dissector_increment_count(cpinfo, proto_tcp);

    src_port = pntoh16(&pd[offset]);
    dst_port = pntoh16(&pd[offset+2]);

    if (src_port > dst_port) {
        low_port = dst_port;
        high_port = src_port;
    } else {
        low_port = src_port;
        high_port = dst_port;
    }

    if (low_port != 0 &&
        try_capture_dissector("tcp.port", low_port, pd, offset+20, len, cpinfo, pseudo_header))
        return TRUE;

    if (high_port != 0 &&
        try_capture_dissector("tcp.port", high_port, pd, offset+20, len, cpinfo, pseudo_header))
        return TRUE;

    /* We've at least identified one type of packet, so this shouldn't be "other" */
    return TRUE;
}