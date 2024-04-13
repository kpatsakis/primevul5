static OPJ_BOOL opj_j2k_write_coc(opj_j2k_t *p_j2k,
                                  OPJ_UINT32 p_comp_no,
                                  opj_stream_private_t *p_stream,
                                  opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 l_coc_size, l_remaining_size;
    OPJ_UINT32 l_comp_room;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_comp_room = (p_j2k->m_private_image->numcomps <= 256) ? 1 : 2;

    l_coc_size = 5 + l_comp_room + opj_j2k_get_SPCod_SPCoc_size(p_j2k,
                 p_j2k->m_current_tile_number, p_comp_no);

    if (l_coc_size > p_j2k->m_specific_param.m_encoder.m_header_tile_data_size) {
        OPJ_BYTE *new_header_tile_data;
        /*p_j2k->m_specific_param.m_encoder.m_header_tile_data
                = (OPJ_BYTE*)opj_realloc(
                        p_j2k->m_specific_param.m_encoder.m_header_tile_data,
                        l_coc_size);*/

        new_header_tile_data = (OPJ_BYTE *) opj_realloc(
                                   p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_coc_size);
        if (! new_header_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
            p_j2k->m_specific_param.m_encoder.m_header_tile_data = NULL;
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to write COC marker\n");
            return OPJ_FALSE;
        }
        p_j2k->m_specific_param.m_encoder.m_header_tile_data = new_header_tile_data;
        p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = l_coc_size;
    }

    opj_j2k_write_coc_in_memory(p_j2k, p_comp_no,
                                p_j2k->m_specific_param.m_encoder.m_header_tile_data, &l_remaining_size,
                                p_manager);

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_coc_size,
                              p_manager) != l_coc_size) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}