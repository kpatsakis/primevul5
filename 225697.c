rvbd_probe_resp_add_info(proto_item *pitem, packet_info *pinfo, tvbuff_t *tvb, int ip_offset, guint16 port)
{
    proto_item_append_text(pitem, ", Server Steelhead: %s:%u", tvb_ip_to_str(tvb, ip_offset), port);

    col_prepend_fstr(pinfo->cinfo, COL_INFO, "SA+, ");
}