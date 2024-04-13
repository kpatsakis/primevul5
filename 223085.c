static void opj_j2k_write_coc_in_memory(opj_j2k_t *p_j2k,
                                        OPJ_UINT32 p_comp_no,
                                        OPJ_BYTE * p_data,
                                        OPJ_UINT32 * p_data_written,
                                        opj_event_mgr_t * p_manager
                                       )
{
    opj_cp_t *l_cp = 00;
    opj_tcp_t *l_tcp = 00;
    OPJ_UINT32 l_coc_size, l_remaining_size;
    OPJ_BYTE * l_current_data = 00;
    opj_image_t *l_image = 00;
    OPJ_UINT32 l_comp_room;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = &l_cp->tcps[p_j2k->m_current_tile_number];
    l_image = p_j2k->m_private_image;
    l_comp_room = (l_image->numcomps <= 256) ? 1 : 2;

    l_coc_size = 5 + l_comp_room + opj_j2k_get_SPCod_SPCoc_size(p_j2k,
                 p_j2k->m_current_tile_number, p_comp_no);
    l_remaining_size = l_coc_size;

    l_current_data = p_data;

    opj_write_bytes(l_current_data, J2K_MS_COC,
                    2);                         /* COC */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_coc_size - 2,
                    2);                     /* L_COC */
    l_current_data += 2;

    opj_write_bytes(l_current_data, p_comp_no, l_comp_room);        /* Ccoc */
    l_current_data += l_comp_room;

    opj_write_bytes(l_current_data, l_tcp->tccps[p_comp_no].csty,
                    1);               /* Scoc */
    ++l_current_data;

    l_remaining_size -= (5 + l_comp_room);
    opj_j2k_write_SPCod_SPCoc(p_j2k, p_j2k->m_current_tile_number, 0,
                              l_current_data, &l_remaining_size, p_manager);
    * p_data_written = l_coc_size;
}