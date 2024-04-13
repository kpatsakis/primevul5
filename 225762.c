tcp_filter_valid(packet_info *pinfo)
{
    return proto_is_frame_protocol(pinfo->layers, "tcp");
}