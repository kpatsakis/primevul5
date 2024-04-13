static OPJ_BOOL opj_j2k_read_header_procedure(opj_j2k_t *p_j2k,
        opj_stream_private_t *p_stream,
        opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 l_current_marker;
    OPJ_UINT32 l_marker_size;
    const opj_dec_memory_marker_handler_t * l_marker_handler = 00;
    OPJ_BOOL l_has_siz = 0;
    OPJ_BOOL l_has_cod = 0;
    OPJ_BOOL l_has_qcd = 0;

    /* preconditions */
    assert(p_stream != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    /*  We enter in the main header */
    p_j2k->m_specific_param.m_decoder.m_state = J2K_STATE_MHSOC;

    /* Try to read the SOC marker, the codestream must begin with SOC marker */
    if (! opj_j2k_read_soc(p_j2k, p_stream, p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR, "Expected a SOC marker \n");
        return OPJ_FALSE;
    }

    /* Try to read 2 bytes (the next marker ID) from stream and copy them into the buffer */
    if (opj_stream_read_data(p_stream,
                             p_j2k->m_specific_param.m_decoder.m_header_data, 2, p_manager) != 2) {
        opj_event_msg(p_manager, EVT_ERROR, "Stream too short\n");
        return OPJ_FALSE;
    }

    /* Read 2 bytes as the new marker ID */
    opj_read_bytes(p_j2k->m_specific_param.m_decoder.m_header_data,
                   &l_current_marker, 2);

    /* Try to read until the SOT is detected */
    while (l_current_marker != J2K_MS_SOT) {

        /* Check if the current marker ID is valid */
        if (l_current_marker < 0xff00) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "A marker ID was expected (0xff--) instead of %.8x\n", l_current_marker);
            return OPJ_FALSE;
        }

        /* Get the marker handler from the marker ID */
        l_marker_handler = opj_j2k_get_marker_handler(l_current_marker);

        /* Manage case where marker is unknown */
        if (l_marker_handler->id == J2K_MS_UNK) {
            if (! opj_j2k_read_unk(p_j2k, p_stream, &l_current_marker, p_manager)) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Unknow marker have been detected and generated error.\n");
                return OPJ_FALSE;
            }

            if (l_current_marker == J2K_MS_SOT) {
                break;    /* SOT marker is detected main header is completely read */
            } else { /* Get the marker handler from the marker ID */
                l_marker_handler = opj_j2k_get_marker_handler(l_current_marker);
            }
        }

        if (l_marker_handler->id == J2K_MS_SIZ) {
            /* Mark required SIZ marker as found */
            l_has_siz = 1;
        }
        if (l_marker_handler->id == J2K_MS_COD) {
            /* Mark required COD marker as found */
            l_has_cod = 1;
        }
        if (l_marker_handler->id == J2K_MS_QCD) {
            /* Mark required QCD marker as found */
            l_has_qcd = 1;
        }

        /* Check if the marker is known and if it is the right place to find it */
        if (!(p_j2k->m_specific_param.m_decoder.m_state & l_marker_handler->states)) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Marker is not compliant with its position\n");
            return OPJ_FALSE;
        }

        /* Try to read 2 bytes (the marker size) from stream and copy them into the buffer */
        if (opj_stream_read_data(p_stream,
                                 p_j2k->m_specific_param.m_decoder.m_header_data, 2, p_manager) != 2) {
            opj_event_msg(p_manager, EVT_ERROR, "Stream too short\n");
            return OPJ_FALSE;
        }

        /* read 2 bytes as the marker size */
        opj_read_bytes(p_j2k->m_specific_param.m_decoder.m_header_data, &l_marker_size,
                       2);
        if (l_marker_size < 2) {
            opj_event_msg(p_manager, EVT_ERROR, "Invalid marker size\n");
            return OPJ_FALSE;
        }
        l_marker_size -= 2; /* Subtract the size of the marker ID already read */

        /* Check if the marker size is compatible with the header data size */
        if (l_marker_size > p_j2k->m_specific_param.m_decoder.m_header_data_size) {
            OPJ_BYTE *new_header_data = (OPJ_BYTE *) opj_realloc(
                                            p_j2k->m_specific_param.m_decoder.m_header_data, l_marker_size);
            if (! new_header_data) {
                opj_free(p_j2k->m_specific_param.m_decoder.m_header_data);
                p_j2k->m_specific_param.m_decoder.m_header_data = NULL;
                p_j2k->m_specific_param.m_decoder.m_header_data_size = 0;
                opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read header\n");
                return OPJ_FALSE;
            }
            p_j2k->m_specific_param.m_decoder.m_header_data = new_header_data;
            p_j2k->m_specific_param.m_decoder.m_header_data_size = l_marker_size;
        }

        /* Try to read the rest of the marker segment from stream and copy them into the buffer */
        if (opj_stream_read_data(p_stream,
                                 p_j2k->m_specific_param.m_decoder.m_header_data, l_marker_size,
                                 p_manager) != l_marker_size) {
            opj_event_msg(p_manager, EVT_ERROR, "Stream too short\n");
            return OPJ_FALSE;
        }

        /* Read the marker segment with the correct marker handler */
        if (!(*(l_marker_handler->handler))(p_j2k,
                                            p_j2k->m_specific_param.m_decoder.m_header_data, l_marker_size, p_manager)) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Marker handler function failed to read the marker segment\n");
            return OPJ_FALSE;
        }

        /* Add the marker to the codestream index*/
        if (OPJ_FALSE == opj_j2k_add_mhmarker(
                    p_j2k->cstr_index,
                    l_marker_handler->id,
                    (OPJ_UINT32) opj_stream_tell(p_stream) - l_marker_size - 4,
                    l_marker_size + 4)) {
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to add mh marker\n");
            return OPJ_FALSE;
        }

        /* Try to read 2 bytes (the next marker ID) from stream and copy them into the buffer */
        if (opj_stream_read_data(p_stream,
                                 p_j2k->m_specific_param.m_decoder.m_header_data, 2, p_manager) != 2) {
            opj_event_msg(p_manager, EVT_ERROR, "Stream too short\n");
            return OPJ_FALSE;
        }

        /* read 2 bytes as the new marker ID */
        opj_read_bytes(p_j2k->m_specific_param.m_decoder.m_header_data,
                       &l_current_marker, 2);
    }

    if (l_has_siz == 0) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "required SIZ marker not found in main header\n");
        return OPJ_FALSE;
    }
    if (l_has_cod == 0) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "required COD marker not found in main header\n");
        return OPJ_FALSE;
    }
    if (l_has_qcd == 0) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "required QCD marker not found in main header\n");
        return OPJ_FALSE;
    }

    if (! opj_j2k_merge_ppm(&(p_j2k->m_cp), p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR, "Failed to merge PPM data\n");
        return OPJ_FALSE;
    }

    opj_event_msg(p_manager, EVT_INFO, "Main header has been correctly decoded.\n");

    /* Position of the last element if the main header */
    p_j2k->cstr_index->main_head_end = (OPJ_UINT32) opj_stream_tell(p_stream) - 2;

    /* Next step: read a tile-part header */
    p_j2k->m_specific_param.m_decoder.m_state = J2K_STATE_TPHSOT;

    return OPJ_TRUE;
}