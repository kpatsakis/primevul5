static OPJ_BOOL opj_j2k_read_mct(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 i;
    opj_tcp_t *l_tcp = 00;
    OPJ_UINT32 l_tmp;
    OPJ_UINT32 l_indix;
    opj_mct_data_t * l_mct_data;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);

    l_tcp = p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH ?
            &p_j2k->m_cp.tcps[p_j2k->m_current_tile_number] :
            p_j2k->m_specific_param.m_decoder.m_default_tcp;

    if (p_header_size < 2) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading MCT marker\n");
        return OPJ_FALSE;
    }

    /* first marker */
    opj_read_bytes(p_header_data, &l_tmp, 2);                       /* Zmct */
    p_header_data += 2;
    if (l_tmp != 0) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "Cannot take in charge mct data within multiple MCT records\n");
        return OPJ_TRUE;
    }

    if (p_header_size <= 6) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading MCT marker\n");
        return OPJ_FALSE;
    }

    /* Imct -> no need for other values, take the first, type is double with decorrelation x0000 1101 0000 0000*/
    opj_read_bytes(p_header_data, &l_tmp, 2);                       /* Imct */
    p_header_data += 2;

    l_indix = l_tmp & 0xff;
    l_mct_data = l_tcp->m_mct_records;

    for (i = 0; i < l_tcp->m_nb_mct_records; ++i) {
        if (l_mct_data->m_index == l_indix) {
            break;
        }
        ++l_mct_data;
    }

    /* NOT FOUND */
    if (i == l_tcp->m_nb_mct_records) {
        if (l_tcp->m_nb_mct_records == l_tcp->m_nb_max_mct_records) {
            opj_mct_data_t *new_mct_records;
            l_tcp->m_nb_max_mct_records += OPJ_J2K_MCT_DEFAULT_NB_RECORDS;

            new_mct_records = (opj_mct_data_t *) opj_realloc(l_tcp->m_mct_records,
                              l_tcp->m_nb_max_mct_records * sizeof(opj_mct_data_t));
            if (! new_mct_records) {
                opj_free(l_tcp->m_mct_records);
                l_tcp->m_mct_records = NULL;
                l_tcp->m_nb_max_mct_records = 0;
                l_tcp->m_nb_mct_records = 0;
                opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read MCT marker\n");
                return OPJ_FALSE;
            }

            /* Update m_mcc_records[].m_offset_array and m_decorrelation_array
             * to point to the new addresses */
            if (new_mct_records != l_tcp->m_mct_records) {
                for (i = 0; i < l_tcp->m_nb_mcc_records; ++i) {
                    opj_simple_mcc_decorrelation_data_t* l_mcc_record =
                        &(l_tcp->m_mcc_records[i]);
                    if (l_mcc_record->m_decorrelation_array) {
                        l_mcc_record->m_decorrelation_array =
                            new_mct_records +
                            (l_mcc_record->m_decorrelation_array -
                             l_tcp->m_mct_records);
                    }
                    if (l_mcc_record->m_offset_array) {
                        l_mcc_record->m_offset_array =
                            new_mct_records +
                            (l_mcc_record->m_offset_array -
                             l_tcp->m_mct_records);
                    }
                }
            }

            l_tcp->m_mct_records = new_mct_records;
            l_mct_data = l_tcp->m_mct_records + l_tcp->m_nb_mct_records;
            memset(l_mct_data, 0, (l_tcp->m_nb_max_mct_records - l_tcp->m_nb_mct_records) *
                   sizeof(opj_mct_data_t));
        }

        l_mct_data = l_tcp->m_mct_records + l_tcp->m_nb_mct_records;
        ++l_tcp->m_nb_mct_records;
    }

    if (l_mct_data->m_data) {
        opj_free(l_mct_data->m_data);
        l_mct_data->m_data = 00;
        l_mct_data->m_data_size = 0;
    }

    l_mct_data->m_index = l_indix;
    l_mct_data->m_array_type = (J2K_MCT_ARRAY_TYPE)((l_tmp  >> 8) & 3);
    l_mct_data->m_element_type = (J2K_MCT_ELEMENT_TYPE)((l_tmp  >> 10) & 3);

    opj_read_bytes(p_header_data, &l_tmp, 2);                       /* Ymct */
    p_header_data += 2;
    if (l_tmp != 0) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "Cannot take in charge multiple MCT markers\n");
        return OPJ_TRUE;
    }

    p_header_size -= 6;

    l_mct_data->m_data = (OPJ_BYTE*)opj_malloc(p_header_size);
    if (! l_mct_data->m_data) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading MCT marker\n");
        return OPJ_FALSE;
    }
    memcpy(l_mct_data->m_data, p_header_data, p_header_size);

    l_mct_data->m_data_size = p_header_size;

    return OPJ_TRUE;
}