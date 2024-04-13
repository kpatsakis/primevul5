static void opj_j2k_tcp_destroy(opj_tcp_t *p_tcp)
{
    if (p_tcp == 00) {
        return;
    }

    if (p_tcp->ppt_markers != 00) {
        OPJ_UINT32 i;
        for (i = 0U; i < p_tcp->ppt_markers_count; ++i) {
            if (p_tcp->ppt_markers[i].m_data != NULL) {
                opj_free(p_tcp->ppt_markers[i].m_data);
            }
        }
        p_tcp->ppt_markers_count = 0U;
        opj_free(p_tcp->ppt_markers);
        p_tcp->ppt_markers = NULL;
    }

    if (p_tcp->ppt_buffer != 00) {
        opj_free(p_tcp->ppt_buffer);
        p_tcp->ppt_buffer = 00;
    }

    if (p_tcp->tccps != 00) {
        opj_free(p_tcp->tccps);
        p_tcp->tccps = 00;
    }

    if (p_tcp->m_mct_coding_matrix != 00) {
        opj_free(p_tcp->m_mct_coding_matrix);
        p_tcp->m_mct_coding_matrix = 00;
    }

    if (p_tcp->m_mct_decoding_matrix != 00) {
        opj_free(p_tcp->m_mct_decoding_matrix);
        p_tcp->m_mct_decoding_matrix = 00;
    }

    if (p_tcp->m_mcc_records) {
        opj_free(p_tcp->m_mcc_records);
        p_tcp->m_mcc_records = 00;
        p_tcp->m_nb_max_mcc_records = 0;
        p_tcp->m_nb_mcc_records = 0;
    }

    if (p_tcp->m_mct_records) {
        opj_mct_data_t * l_mct_data = p_tcp->m_mct_records;
        OPJ_UINT32 i;

        for (i = 0; i < p_tcp->m_nb_mct_records; ++i) {
            if (l_mct_data->m_data) {
                opj_free(l_mct_data->m_data);
                l_mct_data->m_data = 00;
            }

            ++l_mct_data;
        }

        opj_free(p_tcp->m_mct_records);
        p_tcp->m_mct_records = 00;
    }

    if (p_tcp->mct_norms != 00) {
        opj_free(p_tcp->mct_norms);
        p_tcp->mct_norms = 00;
    }

    opj_j2k_tcp_data_destroy(p_tcp);

}