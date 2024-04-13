tcp_dst_prompt(packet_info *pinfo, gchar *result)
{
    guint32 port = GPOINTER_TO_UINT(p_get_proto_data(pinfo->pool, pinfo, hf_tcp_dstport, pinfo->curr_layer_num));

    g_snprintf(result, MAX_DECODE_AS_PROMPT_LEN, "destination (%s%u)", UTF8_RIGHTWARDS_ARROW, port);
}