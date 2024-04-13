static OPJ_BOOL opj_j2k_write_tlm(opj_j2k_t *p_j2k,
                                  opj_stream_private_t *p_stream,
                                  opj_event_mgr_t * p_manager
                                 )
{
    OPJ_BYTE * l_current_data = 00;
    OPJ_UINT32 l_tlm_size;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_tlm_size = 6 + (5 * p_j2k->m_specific_param.m_encoder.m_total_tile_parts);

    if (l_tlm_size > p_j2k->m_specific_param.m_encoder.m_header_tile_data_size) {
        OPJ_BYTE *new_header_tile_data = (OPJ_BYTE *) opj_realloc(
                                             p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_tlm_size);
        if (! new_header_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
            p_j2k->m_specific_param.m_encoder.m_header_tile_data = NULL;
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to write TLM marker\n");
            return OPJ_FALSE;
        }
        p_j2k->m_specific_param.m_encoder.m_header_tile_data = new_header_tile_data;
        p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = l_tlm_size;
    }

    l_current_data = p_j2k->m_specific_param.m_encoder.m_header_tile_data;

    /* change the way data is written to avoid seeking if possible */
    /* TODO */
    p_j2k->m_specific_param.m_encoder.m_tlm_start = opj_stream_tell(p_stream);

    opj_write_bytes(l_current_data, J2K_MS_TLM,
                    2);                                   /* TLM */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_tlm_size - 2,
                    2);                                 /* Lpoc */
    l_current_data += 2;

    opj_write_bytes(l_current_data, 0,
                    1);                                                    /* Ztlm=0*/
    ++l_current_data;

    opj_write_bytes(l_current_data, 0x50,
                    1);                                                 /* Stlm ST=1(8bits-255 tiles max),SP=1(Ptlm=32bits) */
    ++l_current_data;

    /* do nothing on the 5 * l_j2k->m_specific_param.m_encoder.m_total_tile_parts remaining data */
    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_tlm_size,
                              p_manager) != l_tlm_size) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}