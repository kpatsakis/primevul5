static OPJ_BOOL opj_j2k_write_mct_record(opj_j2k_t *p_j2k,
        opj_mct_data_t * p_mct_record,
        struct opj_stream_private *p_stream,
        struct opj_event_mgr * p_manager)
{
    OPJ_UINT32 l_mct_size;
    OPJ_BYTE * l_current_data = 00;
    OPJ_UINT32 l_tmp;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_mct_size = 10 + p_mct_record->m_data_size;

    if (l_mct_size > p_j2k->m_specific_param.m_encoder.m_header_tile_data_size) {
        OPJ_BYTE *new_header_tile_data = (OPJ_BYTE *) opj_realloc(
                                             p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_mct_size);
        if (! new_header_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
            p_j2k->m_specific_param.m_encoder.m_header_tile_data = NULL;
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to write MCT marker\n");
            return OPJ_FALSE;
        }
        p_j2k->m_specific_param.m_encoder.m_header_tile_data = new_header_tile_data;
        p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = l_mct_size;
    }

    l_current_data = p_j2k->m_specific_param.m_encoder.m_header_tile_data;

    opj_write_bytes(l_current_data, J2K_MS_MCT,
                    2);                                   /* MCT */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_mct_size - 2,
                    2);                                 /* Lmct */
    l_current_data += 2;

    opj_write_bytes(l_current_data, 0,
                    2);                                                    /* Zmct */
    l_current_data += 2;

    /* only one marker atm */
    l_tmp = (p_mct_record->m_index & 0xff) | (p_mct_record->m_array_type << 8) |
            (p_mct_record->m_element_type << 10);

    opj_write_bytes(l_current_data, l_tmp, 2);
    l_current_data += 2;

    opj_write_bytes(l_current_data, 0,
                    2);                                                    /* Ymct */
    l_current_data += 2;

    memcpy(l_current_data, p_mct_record->m_data, p_mct_record->m_data_size);

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_mct_size,
                              p_manager) != l_mct_size) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}