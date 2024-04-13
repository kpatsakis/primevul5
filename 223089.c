static OPJ_BOOL opj_j2k_write_cod(opj_j2k_t *p_j2k,
                                  opj_stream_private_t *p_stream,
                                  opj_event_mgr_t * p_manager)
{
    opj_cp_t *l_cp = 00;
    opj_tcp_t *l_tcp = 00;
    OPJ_UINT32 l_code_size, l_remaining_size;
    OPJ_BYTE * l_current_data = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = &l_cp->tcps[p_j2k->m_current_tile_number];
    l_code_size = 9 + opj_j2k_get_SPCod_SPCoc_size(p_j2k,
                  p_j2k->m_current_tile_number, 0);
    l_remaining_size = l_code_size;

    if (l_code_size > p_j2k->m_specific_param.m_encoder.m_header_tile_data_size) {
        OPJ_BYTE *new_header_tile_data = (OPJ_BYTE *) opj_realloc(
                                             p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_code_size);
        if (! new_header_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
            p_j2k->m_specific_param.m_encoder.m_header_tile_data = NULL;
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to write COD marker\n");
            return OPJ_FALSE;
        }
        p_j2k->m_specific_param.m_encoder.m_header_tile_data = new_header_tile_data;
        p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = l_code_size;
    }

    l_current_data = p_j2k->m_specific_param.m_encoder.m_header_tile_data;

    opj_write_bytes(l_current_data, J2K_MS_COD, 2);           /* COD */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_code_size - 2, 2);      /* L_COD */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_tcp->csty, 1);          /* Scod */
    ++l_current_data;

    opj_write_bytes(l_current_data, (OPJ_UINT32)l_tcp->prg, 1); /* SGcod (A) */
    ++l_current_data;

    opj_write_bytes(l_current_data, l_tcp->numlayers, 2);     /* SGcod (B) */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_tcp->mct, 1);           /* SGcod (C) */
    ++l_current_data;

    l_remaining_size -= 9;

    if (! opj_j2k_write_SPCod_SPCoc(p_j2k, p_j2k->m_current_tile_number, 0,
                                    l_current_data, &l_remaining_size, p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR, "Error writing COD marker\n");
        return OPJ_FALSE;
    }

    if (l_remaining_size != 0) {
        opj_event_msg(p_manager, EVT_ERROR, "Error writing COD marker\n");
        return OPJ_FALSE;
    }

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_code_size,
                              p_manager) != l_code_size) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}