static void opj_j2k_write_qcc_in_memory(opj_j2k_t *p_j2k,
                                        OPJ_UINT32 p_comp_no,
                                        OPJ_BYTE * p_data,
                                        OPJ_UINT32 * p_data_written,
                                        opj_event_mgr_t * p_manager
                                       )
{
    OPJ_UINT32 l_qcc_size, l_remaining_size;
    OPJ_BYTE * l_current_data = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_qcc_size = 6 + opj_j2k_get_SQcd_SQcc_size(p_j2k, p_j2k->m_current_tile_number,
                 p_comp_no);
    l_remaining_size = l_qcc_size;

    l_current_data = p_data;

    opj_write_bytes(l_current_data, J2K_MS_QCC, 2);         /* QCC */
    l_current_data += 2;

    if (p_j2k->m_private_image->numcomps <= 256) {
        --l_qcc_size;

        opj_write_bytes(l_current_data, l_qcc_size - 2, 2);     /* L_QCC */
        l_current_data += 2;

        opj_write_bytes(l_current_data, p_comp_no, 1);  /* Cqcc */
        ++l_current_data;

        /* in the case only one byte is sufficient the last byte allocated is useless -> still do -6 for available */
        l_remaining_size -= 6;
    } else {
        opj_write_bytes(l_current_data, l_qcc_size - 2, 2);     /* L_QCC */
        l_current_data += 2;

        opj_write_bytes(l_current_data, p_comp_no, 2);  /* Cqcc */
        l_current_data += 2;

        l_remaining_size -= 6;
    }

    opj_j2k_write_SQcd_SQcc(p_j2k, p_j2k->m_current_tile_number, p_comp_no,
                            l_current_data, &l_remaining_size, p_manager);

    *p_data_written = l_qcc_size;
}