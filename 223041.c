static OPJ_BOOL opj_j2k_write_mcc_record(opj_j2k_t *p_j2k,
        struct opj_simple_mcc_decorrelation_data * p_mcc_record,
        struct opj_stream_private *p_stream,
        struct opj_event_mgr * p_manager)
{
    OPJ_UINT32 i;
    OPJ_UINT32 l_mcc_size;
    OPJ_BYTE * l_current_data = 00;
    OPJ_UINT32 l_nb_bytes_for_comp;
    OPJ_UINT32 l_mask;
    OPJ_UINT32 l_tmcc;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    if (p_mcc_record->m_nb_comps > 255) {
        l_nb_bytes_for_comp = 2;
        l_mask = 0x8000;
    } else {
        l_nb_bytes_for_comp = 1;
        l_mask = 0;
    }

    l_mcc_size = p_mcc_record->m_nb_comps * 2 * l_nb_bytes_for_comp + 19;
    if (l_mcc_size > p_j2k->m_specific_param.m_encoder.m_header_tile_data_size) {
        OPJ_BYTE *new_header_tile_data = (OPJ_BYTE *) opj_realloc(
                                             p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_mcc_size);
        if (! new_header_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
            p_j2k->m_specific_param.m_encoder.m_header_tile_data = NULL;
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to write MCC marker\n");
            return OPJ_FALSE;
        }
        p_j2k->m_specific_param.m_encoder.m_header_tile_data = new_header_tile_data;
        p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = l_mcc_size;
    }

    l_current_data = p_j2k->m_specific_param.m_encoder.m_header_tile_data;

    opj_write_bytes(l_current_data, J2K_MS_MCC,
                    2);                                   /* MCC */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_mcc_size - 2,
                    2);                                 /* Lmcc */
    l_current_data += 2;

    /* first marker */
    opj_write_bytes(l_current_data, 0,
                    2);                                  /* Zmcc */
    l_current_data += 2;

    opj_write_bytes(l_current_data, p_mcc_record->m_index,
                    1);                                        /* Imcc -> no need for other values, take the first */
    ++l_current_data;

    /* only one marker atm */
    opj_write_bytes(l_current_data, 0,
                    2);                                  /* Ymcc */
    l_current_data += 2;

    opj_write_bytes(l_current_data, 1,
                    2);                                  /* Qmcc -> number of collections -> 1 */
    l_current_data += 2;

    opj_write_bytes(l_current_data, 0x1,
                    1);                                /* Xmcci type of component transformation -> array based decorrelation */
    ++l_current_data;

    opj_write_bytes(l_current_data, p_mcc_record->m_nb_comps | l_mask,
                    2);  /* Nmcci number of input components involved and size for each component offset = 8 bits */
    l_current_data += 2;

    for (i = 0; i < p_mcc_record->m_nb_comps; ++i) {
        opj_write_bytes(l_current_data, i,
                        l_nb_bytes_for_comp);                          /* Cmccij Component offset*/
        l_current_data += l_nb_bytes_for_comp;
    }

    opj_write_bytes(l_current_data, p_mcc_record->m_nb_comps | l_mask,
                    2);  /* Mmcci number of output components involved and size for each component offset = 8 bits */
    l_current_data += 2;

    for (i = 0; i < p_mcc_record->m_nb_comps; ++i) {
        opj_write_bytes(l_current_data, i,
                        l_nb_bytes_for_comp);                          /* Wmccij Component offset*/
        l_current_data += l_nb_bytes_for_comp;
    }

    l_tmcc = ((!p_mcc_record->m_is_irreversible) & 1U) << 16;

    if (p_mcc_record->m_decorrelation_array) {
        l_tmcc |= p_mcc_record->m_decorrelation_array->m_index;
    }

    if (p_mcc_record->m_offset_array) {
        l_tmcc |= ((p_mcc_record->m_offset_array->m_index) << 8);
    }

    opj_write_bytes(l_current_data, l_tmcc,
                    3);     /* Tmcci : use MCT defined as number 1 and irreversible array based. */
    l_current_data += 3;

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_mcc_size,
                              p_manager) != l_mcc_size) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}