static OPJ_BOOL opj_tcd_t2_encode(opj_tcd_t *p_tcd,
                                  OPJ_BYTE * p_dest_data,
                                  OPJ_UINT32 * p_data_written,
                                  OPJ_UINT32 p_max_dest_size,
                                  opj_codestream_info_t *p_cstr_info,
                                  opj_tcd_marker_info_t* p_marker_info,
                                  opj_event_mgr_t *p_manager)
{
    opj_t2_t * l_t2;

    l_t2 = opj_t2_create(p_tcd->image, p_tcd->cp);
    if (l_t2 == 00) {
        return OPJ_FALSE;
    }

    if (! opj_t2_encode_packets(
                l_t2,
                p_tcd->tcd_tileno,
                p_tcd->tcd_image->tiles,
                p_tcd->tcp->numlayers,
                p_dest_data,
                p_data_written,
                p_max_dest_size,
                p_cstr_info,
                p_marker_info,
                p_tcd->tp_num,
                p_tcd->tp_pos,
                p_tcd->cur_pino,
                FINAL_PASS,
                p_manager)) {
        opj_t2_destroy(l_t2);
        return OPJ_FALSE;
    }

    opj_t2_destroy(l_t2);

    /*---------------CLEAN-------------------*/
    return OPJ_TRUE;
}