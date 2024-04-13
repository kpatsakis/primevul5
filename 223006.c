static OPJ_BOOL opj_j2k_destroy_header_memory(opj_j2k_t * p_j2k,
        opj_stream_private_t *p_stream,
        opj_event_mgr_t * p_manager
                                             )
{
    /* preconditions */
    assert(p_j2k != 00);
    assert(p_stream != 00);
    assert(p_manager != 00);

    OPJ_UNUSED(p_stream);
    OPJ_UNUSED(p_manager);

    if (p_j2k->m_specific_param.m_encoder.m_header_tile_data) {
        opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
        p_j2k->m_specific_param.m_encoder.m_header_tile_data = 0;
    }

    p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;

    return OPJ_TRUE;
}