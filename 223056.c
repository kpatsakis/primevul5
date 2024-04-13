OPJ_BOOL opj_j2k_setup_mct_encoding(opj_tcp_t * p_tcp, opj_image_t * p_image)
{
    OPJ_UINT32 i;
    OPJ_UINT32 l_indix = 1;
    opj_mct_data_t * l_mct_deco_data = 00, * l_mct_offset_data = 00;
    opj_simple_mcc_decorrelation_data_t * l_mcc_data;
    OPJ_UINT32 l_mct_size, l_nb_elem;
    OPJ_FLOAT32 * l_data, * l_current_data;
    opj_tccp_t * l_tccp;

    /* preconditions */
    assert(p_tcp != 00);

    if (p_tcp->mct != 2) {
        return OPJ_TRUE;
    }

    if (p_tcp->m_mct_decoding_matrix) {
        if (p_tcp->m_nb_mct_records == p_tcp->m_nb_max_mct_records) {
            opj_mct_data_t *new_mct_records;
            p_tcp->m_nb_max_mct_records += OPJ_J2K_MCT_DEFAULT_NB_RECORDS;

            new_mct_records = (opj_mct_data_t *) opj_realloc(p_tcp->m_mct_records,
                              p_tcp->m_nb_max_mct_records * sizeof(opj_mct_data_t));
            if (! new_mct_records) {
                opj_free(p_tcp->m_mct_records);
                p_tcp->m_mct_records = NULL;
                p_tcp->m_nb_max_mct_records = 0;
                p_tcp->m_nb_mct_records = 0;
                /* opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to setup mct encoding\n"); */
                return OPJ_FALSE;
            }
            p_tcp->m_mct_records = new_mct_records;
            l_mct_deco_data = p_tcp->m_mct_records + p_tcp->m_nb_mct_records;

            memset(l_mct_deco_data, 0,
                   (p_tcp->m_nb_max_mct_records - p_tcp->m_nb_mct_records) * sizeof(
                       opj_mct_data_t));
        }
        l_mct_deco_data = p_tcp->m_mct_records + p_tcp->m_nb_mct_records;

        if (l_mct_deco_data->m_data) {
            opj_free(l_mct_deco_data->m_data);
            l_mct_deco_data->m_data = 00;
        }

        l_mct_deco_data->m_index = l_indix++;
        l_mct_deco_data->m_array_type = MCT_TYPE_DECORRELATION;
        l_mct_deco_data->m_element_type = MCT_TYPE_FLOAT;
        l_nb_elem = p_image->numcomps * p_image->numcomps;
        l_mct_size = l_nb_elem * MCT_ELEMENT_SIZE[l_mct_deco_data->m_element_type];
        l_mct_deco_data->m_data = (OPJ_BYTE*)opj_malloc(l_mct_size);

        if (! l_mct_deco_data->m_data) {
            return OPJ_FALSE;
        }

        j2k_mct_write_functions_from_float[l_mct_deco_data->m_element_type](
            p_tcp->m_mct_decoding_matrix, l_mct_deco_data->m_data, l_nb_elem);

        l_mct_deco_data->m_data_size = l_mct_size;
        ++p_tcp->m_nb_mct_records;
    }

    if (p_tcp->m_nb_mct_records == p_tcp->m_nb_max_mct_records) {
        opj_mct_data_t *new_mct_records;
        p_tcp->m_nb_max_mct_records += OPJ_J2K_MCT_DEFAULT_NB_RECORDS;
        new_mct_records = (opj_mct_data_t *) opj_realloc(p_tcp->m_mct_records,
                          p_tcp->m_nb_max_mct_records * sizeof(opj_mct_data_t));
        if (! new_mct_records) {
            opj_free(p_tcp->m_mct_records);
            p_tcp->m_mct_records = NULL;
            p_tcp->m_nb_max_mct_records = 0;
            p_tcp->m_nb_mct_records = 0;
            /* opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to setup mct encoding\n"); */
            return OPJ_FALSE;
        }
        p_tcp->m_mct_records = new_mct_records;
        l_mct_offset_data = p_tcp->m_mct_records + p_tcp->m_nb_mct_records;

        memset(l_mct_offset_data, 0,
               (p_tcp->m_nb_max_mct_records - p_tcp->m_nb_mct_records) * sizeof(
                   opj_mct_data_t));

        if (l_mct_deco_data) {
            l_mct_deco_data = l_mct_offset_data - 1;
        }
    }

    l_mct_offset_data = p_tcp->m_mct_records + p_tcp->m_nb_mct_records;

    if (l_mct_offset_data->m_data) {
        opj_free(l_mct_offset_data->m_data);
        l_mct_offset_data->m_data = 00;
    }

    l_mct_offset_data->m_index = l_indix++;
    l_mct_offset_data->m_array_type = MCT_TYPE_OFFSET;
    l_mct_offset_data->m_element_type = MCT_TYPE_FLOAT;
    l_nb_elem = p_image->numcomps;
    l_mct_size = l_nb_elem * MCT_ELEMENT_SIZE[l_mct_offset_data->m_element_type];
    l_mct_offset_data->m_data = (OPJ_BYTE*)opj_malloc(l_mct_size);

    if (! l_mct_offset_data->m_data) {
        return OPJ_FALSE;
    }

    l_data = (OPJ_FLOAT32*)opj_malloc(l_nb_elem * sizeof(OPJ_FLOAT32));
    if (! l_data) {
        opj_free(l_mct_offset_data->m_data);
        l_mct_offset_data->m_data = 00;
        return OPJ_FALSE;
    }

    l_tccp = p_tcp->tccps;
    l_current_data = l_data;

    for (i = 0; i < l_nb_elem; ++i) {
        *(l_current_data++) = (OPJ_FLOAT32)(l_tccp->m_dc_level_shift);
        ++l_tccp;
    }

    j2k_mct_write_functions_from_float[l_mct_offset_data->m_element_type](l_data,
            l_mct_offset_data->m_data, l_nb_elem);

    opj_free(l_data);

    l_mct_offset_data->m_data_size = l_mct_size;

    ++p_tcp->m_nb_mct_records;

    if (p_tcp->m_nb_mcc_records == p_tcp->m_nb_max_mcc_records) {
        opj_simple_mcc_decorrelation_data_t *new_mcc_records;
        p_tcp->m_nb_max_mcc_records += OPJ_J2K_MCT_DEFAULT_NB_RECORDS;
        new_mcc_records = (opj_simple_mcc_decorrelation_data_t *) opj_realloc(
                              p_tcp->m_mcc_records, p_tcp->m_nb_max_mcc_records * sizeof(
                                  opj_simple_mcc_decorrelation_data_t));
        if (! new_mcc_records) {
            opj_free(p_tcp->m_mcc_records);
            p_tcp->m_mcc_records = NULL;
            p_tcp->m_nb_max_mcc_records = 0;
            p_tcp->m_nb_mcc_records = 0;
            /* opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to setup mct encoding\n"); */
            return OPJ_FALSE;
        }
        p_tcp->m_mcc_records = new_mcc_records;
        l_mcc_data = p_tcp->m_mcc_records + p_tcp->m_nb_mcc_records;
        memset(l_mcc_data, 0, (p_tcp->m_nb_max_mcc_records - p_tcp->m_nb_mcc_records) *
               sizeof(opj_simple_mcc_decorrelation_data_t));

    }

    l_mcc_data = p_tcp->m_mcc_records + p_tcp->m_nb_mcc_records;
    l_mcc_data->m_decorrelation_array = l_mct_deco_data;
    l_mcc_data->m_is_irreversible = 1;
    l_mcc_data->m_nb_comps = p_image->numcomps;
    l_mcc_data->m_index = l_indix++;
    l_mcc_data->m_offset_array = l_mct_offset_data;
    ++p_tcp->m_nb_mcc_records;

    return OPJ_TRUE;
}