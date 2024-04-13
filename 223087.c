static OPJ_BOOL opj_j2k_post_write_tile(opj_j2k_t * p_j2k,
                                        opj_stream_private_t *p_stream,
                                        opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 l_nb_bytes_written;
    OPJ_BYTE * l_current_data = 00;
    OPJ_UINT32 l_tile_size = 0;
    OPJ_UINT32 l_available_data;

    /* preconditions */
    assert(p_j2k->m_specific_param.m_encoder.m_encoded_tile_data);

    l_tile_size = p_j2k->m_specific_param.m_encoder.m_encoded_tile_size;
    l_available_data = l_tile_size;
    l_current_data = p_j2k->m_specific_param.m_encoder.m_encoded_tile_data;

    l_nb_bytes_written = 0;
    if (! opj_j2k_write_first_tile_part(p_j2k, l_current_data, &l_nb_bytes_written,
                                        l_available_data, p_stream, p_manager)) {
        return OPJ_FALSE;
    }
    l_current_data += l_nb_bytes_written;
    l_available_data -= l_nb_bytes_written;

    l_nb_bytes_written = 0;
    if (! opj_j2k_write_all_tile_parts(p_j2k, l_current_data, &l_nb_bytes_written,
                                       l_available_data, p_stream, p_manager)) {
        return OPJ_FALSE;
    }

    l_available_data -= l_nb_bytes_written;
    l_nb_bytes_written = l_tile_size - l_available_data;

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_encoded_tile_data,
                              l_nb_bytes_written, p_manager) != l_nb_bytes_written) {
        return OPJ_FALSE;
    }

    ++p_j2k->m_current_tile_number;

    return OPJ_TRUE;
}