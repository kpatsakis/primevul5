static OPJ_BOOL opj_j2k_read_eoc(opj_j2k_t *p_j2k,
                                 opj_stream_private_t *p_stream,
                                 opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 i;
    opj_tcd_t * l_tcd = 00;
    OPJ_UINT32 l_nb_tiles;
    opj_tcp_t * l_tcp = 00;
    OPJ_BOOL l_success;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_nb_tiles = p_j2k->m_cp.th * p_j2k->m_cp.tw;
    l_tcp = p_j2k->m_cp.tcps;

    l_tcd = opj_tcd_create(OPJ_TRUE);
    if (l_tcd == 00) {
        opj_event_msg(p_manager, EVT_ERROR, "Cannot decode tile, memory error\n");
        return OPJ_FALSE;
    }

    for (i = 0; i < l_nb_tiles; ++i) {
        if (l_tcp->m_data) {
            if (! opj_tcd_init_decode_tile(l_tcd, i)) {
                opj_tcd_destroy(l_tcd);
                opj_event_msg(p_manager, EVT_ERROR, "Cannot decode tile, memory error\n");
                return OPJ_FALSE;
            }

            l_success = opj_tcd_decode_tile(l_tcd, l_tcp->m_data, l_tcp->m_data_size, i,
                                            p_j2k->cstr_index);
            /* cleanup */

            if (! l_success) {
                p_j2k->m_specific_param.m_decoder.m_state |= J2K_STATE_ERR;
                break;
            }
        }

        opj_j2k_tcp_destroy(l_tcp);
        ++l_tcp;
    }

    opj_tcd_destroy(l_tcd);
    return OPJ_TRUE;
}