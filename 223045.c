static OPJ_BOOL opj_j2k_pre_write_tile(opj_j2k_t * p_j2k,
                                       OPJ_UINT32 p_tile_index,
                                       opj_stream_private_t *p_stream,
                                       opj_event_mgr_t * p_manager)
{
    (void)p_stream;
    if (p_tile_index != p_j2k->m_current_tile_number) {
        opj_event_msg(p_manager, EVT_ERROR, "The given tile index does not match.");
        return OPJ_FALSE;
    }

    opj_event_msg(p_manager, EVT_INFO, "tile number %d / %d\n",
                  p_j2k->m_current_tile_number + 1, p_j2k->m_cp.tw * p_j2k->m_cp.th);

    p_j2k->m_specific_param.m_encoder.m_current_tile_part_number = 0;
    p_j2k->m_tcd->cur_totnum_tp = p_j2k->m_cp.tcps[p_tile_index].m_nb_tile_parts;
    p_j2k->m_specific_param.m_encoder.m_current_poc_tile_part_number = 0;

    /* initialisation before tile encoding  */
    if (! opj_tcd_init_encode_tile(p_j2k->m_tcd, p_j2k->m_current_tile_number,
                                   p_manager)) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}