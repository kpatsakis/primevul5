static OPJ_BOOL opj_j2k_write_mct_data_group(opj_j2k_t *p_j2k,
        struct opj_stream_private *p_stream,
        struct opj_event_mgr * p_manager)
{
    OPJ_UINT32 i;
    opj_simple_mcc_decorrelation_data_t * l_mcc_record;
    opj_mct_data_t * l_mct_record;
    opj_tcp_t * l_tcp;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_stream != 00);
    assert(p_manager != 00);

    if (! opj_j2k_write_cbd(p_j2k, p_stream, p_manager)) {
        return OPJ_FALSE;
    }

    l_tcp = &(p_j2k->m_cp.tcps[p_j2k->m_current_tile_number]);
    l_mct_record = l_tcp->m_mct_records;

    for (i = 0; i < l_tcp->m_nb_mct_records; ++i) {

        if (! opj_j2k_write_mct_record(p_j2k, l_mct_record, p_stream, p_manager)) {
            return OPJ_FALSE;
        }

        ++l_mct_record;
    }

    l_mcc_record = l_tcp->m_mcc_records;

    for (i = 0; i < l_tcp->m_nb_mcc_records; ++i) {

        if (! opj_j2k_write_mcc_record(p_j2k, l_mcc_record, p_stream, p_manager)) {
            return OPJ_FALSE;
        }

        ++l_mcc_record;
    }

    if (! opj_j2k_write_mco(p_j2k, p_stream, p_manager)) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}