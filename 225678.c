tcp_info_append_uint(packet_info *pinfo, const char *abbrev, guint32 val)
{
    /* fstr(" %s=%u", abbrev, val) */
    col_append_str_uint(pinfo->cinfo, COL_INFO, abbrev, val, " ");
}