static OPJ_BOOL opj_j2k_write_com(opj_j2k_t *p_j2k,
                                  opj_stream_private_t *p_stream,
                                  opj_event_mgr_t * p_manager
                                 )
{
    OPJ_UINT32 l_comment_size;
    OPJ_UINT32 l_total_com_size;
    const OPJ_CHAR *l_comment;
    OPJ_BYTE * l_current_ptr = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_stream != 00);
    assert(p_manager != 00);

    l_comment = p_j2k->m_cp.comment;
    l_comment_size = (OPJ_UINT32)strlen(l_comment);
    l_total_com_size = l_comment_size + 6;

    if (l_total_com_size >
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size) {
        OPJ_BYTE *new_header_tile_data = (OPJ_BYTE *) opj_realloc(
                                             p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_total_com_size);
        if (! new_header_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
            p_j2k->m_specific_param.m_encoder.m_header_tile_data = NULL;
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;
            opj_event_msg(p_manager, EVT_ERROR,
                          "Not enough memory to write the COM marker\n");
            return OPJ_FALSE;
        }
        p_j2k->m_specific_param.m_encoder.m_header_tile_data = new_header_tile_data;
        p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = l_total_com_size;
    }

    l_current_ptr = p_j2k->m_specific_param.m_encoder.m_header_tile_data;

    opj_write_bytes(l_current_ptr, J2K_MS_COM, 2);  /* COM */
    l_current_ptr += 2;

    opj_write_bytes(l_current_ptr, l_total_com_size - 2, 2);        /* L_COM */
    l_current_ptr += 2;

    opj_write_bytes(l_current_ptr, 1,
                    2);   /* General use (IS 8859-15:1999 (Latin) values) */
    l_current_ptr += 2;

    memcpy(l_current_ptr, l_comment, l_comment_size);

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_total_com_size,
                              p_manager) != l_total_com_size) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}