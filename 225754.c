tcp_both_prompt(packet_info *pinfo, gchar *result)
{
    guint32 srcport = GPOINTER_TO_UINT(p_get_proto_data(pinfo->pool, pinfo, hf_tcp_srcport, pinfo->curr_layer_num)),
            destport = GPOINTER_TO_UINT(p_get_proto_data(pinfo->pool, pinfo, hf_tcp_dstport, pinfo->curr_layer_num));
    g_snprintf(result, MAX_DECODE_AS_PROMPT_LEN, "both (%u%s%u)", srcport, UTF8_LEFT_RIGHT_ARROW, destport);
}