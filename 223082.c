static OPJ_BOOL opj_j2k_write_qcd(opj_j2k_t *p_j2k,
                                  opj_stream_private_t *p_stream,
                                  opj_event_mgr_t * p_manager
                                 )
{
    OPJ_UINT32 l_qcd_size, l_remaining_size;
    OPJ_BYTE * l_current_data = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_qcd_size = 4 + opj_j2k_get_SQcd_SQcc_size(p_j2k, p_j2k->m_current_tile_number,
                 0);
    l_remaining_size = l_qcd_size;

    if (l_qcd_size > p_j2k->m_specific_param.m_encoder.m_header_tile_data_size) {
        OPJ_BYTE *new_header_tile_data = (OPJ_BYTE *) opj_realloc(
                                             p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_qcd_size);
        if (! new_header_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
            p_j2k->m_specific_param.m_encoder.m_header_tile_data = NULL;
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to write QCD marker\n");
            return OPJ_FALSE;
        }
        p_j2k->m_specific_param.m_encoder.m_header_tile_data = new_header_tile_data;
        p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = l_qcd_size;
    }

    l_current_data = p_j2k->m_specific_param.m_encoder.m_header_tile_data;

    opj_write_bytes(l_current_data, J2K_MS_QCD, 2);         /* QCD */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_qcd_size - 2, 2);     /* L_QCD */
    l_current_data += 2;

    l_remaining_size -= 4;

    if (! opj_j2k_write_SQcd_SQcc(p_j2k, p_j2k->m_current_tile_number, 0,
                                  l_current_data, &l_remaining_size, p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR, "Error writing QCD marker\n");
        return OPJ_FALSE;
    }

    if (l_remaining_size != 0) {
        opj_event_msg(p_manager, EVT_ERROR, "Error writing QCD marker\n");
        return OPJ_FALSE;
    }

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_qcd_size,
                              p_manager) != l_qcd_size) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}