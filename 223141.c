void opj_tcd_marker_info_destroy(opj_tcd_marker_info_t *p_tcd_marker_info)
{
    if (p_tcd_marker_info) {
        opj_free(p_tcd_marker_info->p_packet_size);
    }
}