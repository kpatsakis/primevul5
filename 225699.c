tcp_src_value(packet_info *pinfo)
{
    return p_get_proto_data(pinfo->pool, pinfo, hf_tcp_srcport, pinfo->curr_layer_num);
}