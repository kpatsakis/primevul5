static OPJ_BOOL opj_j2k_write_rgn(opj_j2k_t *p_j2k,
                                  OPJ_UINT32 p_tile_no,
                                  OPJ_UINT32 p_comp_no,
                                  OPJ_UINT32 nb_comps,
                                  opj_stream_private_t *p_stream,
                                  opj_event_mgr_t * p_manager
                                 )
{
    OPJ_BYTE * l_current_data = 00;
    OPJ_UINT32 l_rgn_size;
    opj_cp_t *l_cp = 00;
    opj_tcp_t *l_tcp = 00;
    opj_tccp_t *l_tccp = 00;
    OPJ_UINT32 l_comp_room;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = &l_cp->tcps[p_tile_no];
    l_tccp = &l_tcp->tccps[p_comp_no];

    if (nb_comps <= 256) {
        l_comp_room = 1;
    } else {
        l_comp_room = 2;
    }

    l_rgn_size = 6 + l_comp_room;

    l_current_data = p_j2k->m_specific_param.m_encoder.m_header_tile_data;

    opj_write_bytes(l_current_data, J2K_MS_RGN,
                    2);                                   /* RGN  */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_rgn_size - 2,
                    2);                                 /* Lrgn */
    l_current_data += 2;

    opj_write_bytes(l_current_data, p_comp_no,
                    l_comp_room);                          /* Crgn */
    l_current_data += l_comp_room;

    opj_write_bytes(l_current_data, 0,
                    1);                                           /* Srgn */
    ++l_current_data;

    opj_write_bytes(l_current_data, (OPJ_UINT32)l_tccp->roishift,
                    1);                            /* SPrgn */
    ++l_current_data;

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_rgn_size,
                              p_manager) != l_rgn_size) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}