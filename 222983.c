static OPJ_BOOL opj_j2k_end_encoding(opj_j2k_t *p_j2k,
                                     struct opj_stream_private *p_stream,
                                     struct opj_event_mgr * p_manager)
{
    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    OPJ_UNUSED(p_stream);
    OPJ_UNUSED(p_manager);

    opj_tcd_destroy(p_j2k->m_tcd);
    p_j2k->m_tcd = 00;

    if (p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_buffer) {
        opj_free(p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_buffer);
        p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_buffer = 0;
        p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_current = 0;
    }

    if (p_j2k->m_specific_param.m_encoder.m_encoded_tile_data) {
        opj_free(p_j2k->m_specific_param.m_encoder.m_encoded_tile_data);
        p_j2k->m_specific_param.m_encoder.m_encoded_tile_data = 0;
    }

    p_j2k->m_specific_param.m_encoder.m_encoded_tile_size = 0;

    return OPJ_TRUE;
}