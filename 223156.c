static OPJ_BOOL opj_tcd_t2_decode(opj_tcd_t *p_tcd,
                                  OPJ_BYTE * p_src_data,
                                  OPJ_UINT32 * p_data_read,
                                  OPJ_UINT32 p_max_src_size,
                                  opj_codestream_index_t *p_cstr_index,
                                  opj_event_mgr_t *p_manager
                                 )
{
    opj_t2_t * l_t2;

    l_t2 = opj_t2_create(p_tcd->image, p_tcd->cp);
    if (l_t2 == 00) {
        return OPJ_FALSE;
    }

    if (! opj_t2_decode_packets(
                p_tcd,
                l_t2,
                p_tcd->tcd_tileno,
                p_tcd->tcd_image->tiles,
                p_src_data,
                p_data_read,
                p_max_src_size,
                p_cstr_index,
                p_manager)) {
        opj_t2_destroy(l_t2);
        return OPJ_FALSE;
    }

    opj_t2_destroy(l_t2);

    /*---------------CLEAN-------------------*/
    return OPJ_TRUE;
}