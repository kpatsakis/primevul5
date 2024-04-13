tcp_dst_value(packet_info *pinfo)
{
    return p_get_proto_data(pinfo->pool, pinfo, hf_tcp_dstport, pinfo->curr_layer_num);
}