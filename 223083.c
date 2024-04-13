static OPJ_BOOL opj_j2k_write_updated_tlm(opj_j2k_t *p_j2k,
        struct opj_stream_private *p_stream,
        struct opj_event_mgr * p_manager)
{
    OPJ_UINT32 l_tlm_size;
    OPJ_OFF_T l_tlm_position, l_current_position;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_tlm_size = 5 * p_j2k->m_specific_param.m_encoder.m_total_tile_parts;
    l_tlm_position = 6 + p_j2k->m_specific_param.m_encoder.m_tlm_start;
    l_current_position = opj_stream_tell(p_stream);

    if (! opj_stream_seek(p_stream, l_tlm_position, p_manager)) {
        return OPJ_FALSE;
    }

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_buffer, l_tlm_size,
                              p_manager) != l_tlm_size) {
        return OPJ_FALSE;
    }

    if (! opj_stream_seek(p_stream, l_current_position, p_manager)) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}