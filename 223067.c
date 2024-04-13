static OPJ_BOOL opj_j2k_read_mcc(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 i, j;
    OPJ_UINT32 l_tmp;
    OPJ_UINT32 l_indix;
    opj_tcp_t * l_tcp;
    opj_simple_mcc_decorrelation_data_t * l_mcc_record;
    opj_mct_data_t * l_mct_data;
    OPJ_UINT32 l_nb_collections;
    OPJ_UINT32 l_nb_comps;
    OPJ_UINT32 l_nb_bytes_by_comp;
    OPJ_BOOL l_new_mcc = OPJ_FALSE;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_tcp = p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH ?
            &p_j2k->m_cp.tcps[p_j2k->m_current_tile_number] :
            p_j2k->m_specific_param.m_decoder.m_default_tcp;

    if (p_header_size < 2) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading MCC marker\n");
        return OPJ_FALSE;
    }

    /* first marker */
    opj_read_bytes(p_header_data, &l_tmp, 2);                       /* Zmcc */
    p_header_data += 2;
    if (l_tmp != 0) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "Cannot take in charge multiple data spanning\n");
        return OPJ_TRUE;
    }

    if (p_header_size < 7) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading MCC marker\n");
        return OPJ_FALSE;
    }

    opj_read_bytes(p_header_data, &l_indix,
                   1); /* Imcc -> no need for other values, take the first */
    ++p_header_data;

    l_mcc_record = l_tcp->m_mcc_records;

    for (i = 0; i < l_tcp->m_nb_mcc_records; ++i) {
        if (l_mcc_record->m_index == l_indix) {
            break;
        }
        ++l_mcc_record;
    }

    /** NOT FOUND */
    if (i == l_tcp->m_nb_mcc_records) {
        if (l_tcp->m_nb_mcc_records == l_tcp->m_nb_max_mcc_records) {
            opj_simple_mcc_decorrelation_data_t *new_mcc_records;
            l_tcp->m_nb_max_mcc_records += OPJ_J2K_MCC_DEFAULT_NB_RECORDS;

            new_mcc_records = (opj_simple_mcc_decorrelation_data_t *) opj_realloc(
                                  l_tcp->m_mcc_records, l_tcp->m_nb_max_mcc_records * sizeof(
                                      opj_simple_mcc_decorrelation_data_t));
            if (! new_mcc_records) {
                opj_free(l_tcp->m_mcc_records);
                l_tcp->m_mcc_records = NULL;
                l_tcp->m_nb_max_mcc_records = 0;
                l_tcp->m_nb_mcc_records = 0;
                opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read MCC marker\n");
                return OPJ_FALSE;
            }
            l_tcp->m_mcc_records = new_mcc_records;
            l_mcc_record = l_tcp->m_mcc_records + l_tcp->m_nb_mcc_records;
            memset(l_mcc_record, 0, (l_tcp->m_nb_max_mcc_records - l_tcp->m_nb_mcc_records)
                   * sizeof(opj_simple_mcc_decorrelation_data_t));
        }
        l_mcc_record = l_tcp->m_mcc_records + l_tcp->m_nb_mcc_records;
        l_new_mcc = OPJ_TRUE;
    }
    l_mcc_record->m_index = l_indix;

    /* only one marker atm */
    opj_read_bytes(p_header_data, &l_tmp, 2);                       /* Ymcc */
    p_header_data += 2;
    if (l_tmp != 0) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "Cannot take in charge multiple data spanning\n");
        return OPJ_TRUE;
    }

    opj_read_bytes(p_header_data, &l_nb_collections,
                   2);                              /* Qmcc -> number of collections -> 1 */
    p_header_data += 2;

    if (l_nb_collections > 1) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "Cannot take in charge multiple collections\n");
        return OPJ_TRUE;
    }

    p_header_size -= 7;

    for (i = 0; i < l_nb_collections; ++i) {
        if (p_header_size < 3) {
            opj_event_msg(p_manager, EVT_ERROR, "Error reading MCC marker\n");
            return OPJ_FALSE;
        }

        opj_read_bytes(p_header_data, &l_tmp,
                       1); /* Xmcci type of component transformation -> array based decorrelation */
        ++p_header_data;

        if (l_tmp != 1) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "Cannot take in charge collections other than array decorrelation\n");
            return OPJ_TRUE;
        }

        opj_read_bytes(p_header_data, &l_nb_comps, 2);

        p_header_data += 2;
        p_header_size -= 3;

        l_nb_bytes_by_comp = 1 + (l_nb_comps >> 15);
        l_mcc_record->m_nb_comps = l_nb_comps & 0x7fff;

        if (p_header_size < (l_nb_bytes_by_comp * l_mcc_record->m_nb_comps + 2)) {
            opj_event_msg(p_manager, EVT_ERROR, "Error reading MCC marker\n");
            return OPJ_FALSE;
        }

        p_header_size -= (l_nb_bytes_by_comp * l_mcc_record->m_nb_comps + 2);

        for (j = 0; j < l_mcc_record->m_nb_comps; ++j) {
            opj_read_bytes(p_header_data, &l_tmp,
                           l_nb_bytes_by_comp);      /* Cmccij Component offset*/
            p_header_data += l_nb_bytes_by_comp;

            if (l_tmp != j) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "Cannot take in charge collections with indix shuffle\n");
                return OPJ_TRUE;
            }
        }

        opj_read_bytes(p_header_data, &l_nb_comps, 2);
        p_header_data += 2;

        l_nb_bytes_by_comp = 1 + (l_nb_comps >> 15);
        l_nb_comps &= 0x7fff;

        if (l_nb_comps != l_mcc_record->m_nb_comps) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "Cannot take in charge collections without same number of indixes\n");
            return OPJ_TRUE;
        }

        if (p_header_size < (l_nb_bytes_by_comp * l_mcc_record->m_nb_comps + 3)) {
            opj_event_msg(p_manager, EVT_ERROR, "Error reading MCC marker\n");
            return OPJ_FALSE;
        }

        p_header_size -= (l_nb_bytes_by_comp * l_mcc_record->m_nb_comps + 3);

        for (j = 0; j < l_mcc_record->m_nb_comps; ++j) {
            opj_read_bytes(p_header_data, &l_tmp,
                           l_nb_bytes_by_comp);      /* Wmccij Component offset*/
            p_header_data += l_nb_bytes_by_comp;

            if (l_tmp != j) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "Cannot take in charge collections with indix shuffle\n");
                return OPJ_TRUE;
            }
        }

        opj_read_bytes(p_header_data, &l_tmp, 3); /* Wmccij Component offset*/
        p_header_data += 3;

        l_mcc_record->m_is_irreversible = !((l_tmp >> 16) & 1);
        l_mcc_record->m_decorrelation_array = 00;
        l_mcc_record->m_offset_array = 00;

        l_indix = l_tmp & 0xff;
        if (l_indix != 0) {
            l_mct_data = l_tcp->m_mct_records;
            for (j = 0; j < l_tcp->m_nb_mct_records; ++j) {
                if (l_mct_data->m_index == l_indix) {
                    l_mcc_record->m_decorrelation_array = l_mct_data;
                    break;
                }
                ++l_mct_data;
            }

            if (l_mcc_record->m_decorrelation_array == 00) {
                opj_event_msg(p_manager, EVT_ERROR, "Error reading MCC marker\n");
                return OPJ_FALSE;
            }
        }

        l_indix = (l_tmp >> 8) & 0xff;
        if (l_indix != 0) {
            l_mct_data = l_tcp->m_mct_records;
            for (j = 0; j < l_tcp->m_nb_mct_records; ++j) {
                if (l_mct_data->m_index == l_indix) {
                    l_mcc_record->m_offset_array = l_mct_data;
                    break;
                }
                ++l_mct_data;
            }

            if (l_mcc_record->m_offset_array == 00) {
                opj_event_msg(p_manager, EVT_ERROR, "Error reading MCC marker\n");
                return OPJ_FALSE;
            }
        }
    }

    if (p_header_size != 0) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading MCC marker\n");
        return OPJ_FALSE;
    }

    if (l_new_mcc) {
        ++l_tcp->m_nb_mcc_records;
    }

    return OPJ_TRUE;
}