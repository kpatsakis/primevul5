static OPJ_BOOL opj_j2k_read_coc(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    opj_cp_t *l_cp = NULL;
    opj_tcp_t *l_tcp = NULL;
    opj_image_t *l_image = NULL;
    OPJ_UINT32 l_comp_room;
    OPJ_UINT32 l_comp_no;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = (p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH)
            ?
            &l_cp->tcps[p_j2k->m_current_tile_number] :
            p_j2k->m_specific_param.m_decoder.m_default_tcp;
    l_image = p_j2k->m_private_image;

    l_comp_room = l_image->numcomps <= 256 ? 1 : 2;

    /* make sure room is sufficient*/
    if (p_header_size < l_comp_room + 1) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading COC marker\n");
        return OPJ_FALSE;
    }
    p_header_size -= l_comp_room + 1;

    opj_read_bytes(p_header_data, &l_comp_no,
                   l_comp_room);                 /* Ccoc */
    p_header_data += l_comp_room;
    if (l_comp_no >= l_image->numcomps) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Error reading COC marker (bad number of components)\n");
        return OPJ_FALSE;
    }

    opj_read_bytes(p_header_data, &l_tcp->tccps[l_comp_no].csty,
                   1);                  /* Scoc */
    ++p_header_data ;

    if (! opj_j2k_read_SPCod_SPCoc(p_j2k, l_comp_no, p_header_data, &p_header_size,
                                   p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading COC marker\n");
        return OPJ_FALSE;
    }

    if (p_header_size != 0) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading COC marker\n");
        return OPJ_FALSE;
    }
    return OPJ_TRUE;
}