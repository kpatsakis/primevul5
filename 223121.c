static OPJ_BOOL opj_j2k_write_poc(opj_j2k_t *p_j2k,
                                  opj_stream_private_t *p_stream,
                                  opj_event_mgr_t * p_manager
                                 )
{
    OPJ_UINT32 l_nb_comp;
    OPJ_UINT32 l_nb_poc;
    OPJ_UINT32 l_poc_size;
    OPJ_UINT32 l_written_size = 0;
    opj_tcp_t *l_tcp = 00;
    OPJ_UINT32 l_poc_room;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_tcp = &p_j2k->m_cp.tcps[p_j2k->m_current_tile_number];
    l_nb_comp = p_j2k->m_private_image->numcomps;
    l_nb_poc = 1 + l_tcp->numpocs;

    if (l_nb_comp <= 256) {
        l_poc_room = 1;
    } else {
        l_poc_room = 2;
    }
    l_poc_size = 4 + (5 + 2 * l_poc_room) * l_nb_poc;

    if (l_poc_size > p_j2k->m_specific_param.m_encoder.m_header_tile_data_size) {
        OPJ_BYTE *new_header_tile_data = (OPJ_BYTE *) opj_realloc(
                                             p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_poc_size);
        if (! new_header_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
            p_j2k->m_specific_param.m_encoder.m_header_tile_data = NULL;
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to write POC marker\n");
            return OPJ_FALSE;
        }
        p_j2k->m_specific_param.m_encoder.m_header_tile_data = new_header_tile_data;
        p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = l_poc_size;
    }

    opj_j2k_write_poc_in_memory(p_j2k,
                                p_j2k->m_specific_param.m_encoder.m_header_tile_data, &l_written_size,
                                p_manager);

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_poc_size,
                              p_manager) != l_poc_size) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}