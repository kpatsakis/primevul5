static OPJ_BOOL opj_j2k_read_mco(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 l_tmp, i;
    OPJ_UINT32 l_nb_stages;
    opj_tcp_t * l_tcp;
    opj_tccp_t * l_tccp;
    opj_image_t * l_image;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_image = p_j2k->m_private_image;
    l_tcp = p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH ?
            &p_j2k->m_cp.tcps[p_j2k->m_current_tile_number] :
            p_j2k->m_specific_param.m_decoder.m_default_tcp;

    if (p_header_size < 1) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading MCO marker\n");
        return OPJ_FALSE;
    }

    opj_read_bytes(p_header_data, &l_nb_stages,
                   1);                         /* Nmco : only one transform stage*/
    ++p_header_data;

    if (l_nb_stages > 1) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "Cannot take in charge multiple transformation stages.\n");
        return OPJ_TRUE;
    }

    if (p_header_size != l_nb_stages + 1) {
        opj_event_msg(p_manager, EVT_WARNING, "Error reading MCO marker\n");
        return OPJ_FALSE;
    }

    l_tccp = l_tcp->tccps;

    for (i = 0; i < l_image->numcomps; ++i) {
        l_tccp->m_dc_level_shift = 0;
        ++l_tccp;
    }

    if (l_tcp->m_mct_decoding_matrix) {
        opj_free(l_tcp->m_mct_decoding_matrix);
        l_tcp->m_mct_decoding_matrix = 00;
    }

    for (i = 0; i < l_nb_stages; ++i) {
        opj_read_bytes(p_header_data, &l_tmp, 1);
        ++p_header_data;

        if (! opj_j2k_add_mct(l_tcp, p_j2k->m_private_image, l_tmp)) {
            return OPJ_FALSE;
        }
    }

    return OPJ_TRUE;
}