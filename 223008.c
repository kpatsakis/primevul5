static OPJ_BOOL opj_j2k_add_mct(opj_tcp_t * p_tcp, opj_image_t * p_image,
                                OPJ_UINT32 p_index)
{
    OPJ_UINT32 i;
    opj_simple_mcc_decorrelation_data_t * l_mcc_record;
    opj_mct_data_t * l_deco_array, * l_offset_array;
    OPJ_UINT32 l_data_size, l_mct_size, l_offset_size;
    OPJ_UINT32 l_nb_elem;
    OPJ_UINT32 * l_offset_data, * l_current_offset_data;
    opj_tccp_t * l_tccp;

    /* preconditions */
    assert(p_tcp != 00);

    l_mcc_record = p_tcp->m_mcc_records;

    for (i = 0; i < p_tcp->m_nb_mcc_records; ++i) {
        if (l_mcc_record->m_index == p_index) {
            break;
        }
    }

    if (i == p_tcp->m_nb_mcc_records) {
        /** element discarded **/
        return OPJ_TRUE;
    }

    if (l_mcc_record->m_nb_comps != p_image->numcomps) {
        /** do not support number of comps != image */
        return OPJ_TRUE;
    }

    l_deco_array = l_mcc_record->m_decorrelation_array;

    if (l_deco_array) {
        l_data_size = MCT_ELEMENT_SIZE[l_deco_array->m_element_type] * p_image->numcomps
                      * p_image->numcomps;
        if (l_deco_array->m_data_size != l_data_size) {
            return OPJ_FALSE;
        }

        l_nb_elem = p_image->numcomps * p_image->numcomps;
        l_mct_size = l_nb_elem * (OPJ_UINT32)sizeof(OPJ_FLOAT32);
        p_tcp->m_mct_decoding_matrix = (OPJ_FLOAT32*)opj_malloc(l_mct_size);

        if (! p_tcp->m_mct_decoding_matrix) {
            return OPJ_FALSE;
        }

        j2k_mct_read_functions_to_float[l_deco_array->m_element_type](
            l_deco_array->m_data, p_tcp->m_mct_decoding_matrix, l_nb_elem);
    }

    l_offset_array = l_mcc_record->m_offset_array;

    if (l_offset_array) {
        l_data_size = MCT_ELEMENT_SIZE[l_offset_array->m_element_type] *
                      p_image->numcomps;
        if (l_offset_array->m_data_size != l_data_size) {
            return OPJ_FALSE;
        }

        l_nb_elem = p_image->numcomps;
        l_offset_size = l_nb_elem * (OPJ_UINT32)sizeof(OPJ_UINT32);
        l_offset_data = (OPJ_UINT32*)opj_malloc(l_offset_size);

        if (! l_offset_data) {
            return OPJ_FALSE;
        }

        j2k_mct_read_functions_to_int32[l_offset_array->m_element_type](
            l_offset_array->m_data, l_offset_data, l_nb_elem);

        l_tccp = p_tcp->tccps;
        l_current_offset_data = l_offset_data;

        for (i = 0; i < p_image->numcomps; ++i) {
            l_tccp->m_dc_level_shift = (OPJ_INT32) * (l_current_offset_data++);
            ++l_tccp;
        }

        opj_free(l_offset_data);
    }

    return OPJ_TRUE;
}