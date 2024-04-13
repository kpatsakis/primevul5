static OPJ_BOOL opj_j2k_read_unk(opj_j2k_t *p_j2k,
                                 opj_stream_private_t *p_stream,
                                 OPJ_UINT32 *output_marker,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 l_unknown_marker;
    const opj_dec_memory_marker_handler_t * l_marker_handler;
    OPJ_UINT32 l_size_unk = 2;

    /* preconditions*/
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    opj_event_msg(p_manager, EVT_WARNING, "Unknown marker\n");

    for (;;) {
        /* Try to read 2 bytes (the next marker ID) from stream and copy them into the buffer*/
        if (opj_stream_read_data(p_stream,
                                 p_j2k->m_specific_param.m_decoder.m_header_data, 2, p_manager) != 2) {
            opj_event_msg(p_manager, EVT_ERROR, "Stream too short\n");
            return OPJ_FALSE;
        }

        /* read 2 bytes as the new marker ID*/
        opj_read_bytes(p_j2k->m_specific_param.m_decoder.m_header_data,
                       &l_unknown_marker, 2);

        if (!(l_unknown_marker < 0xff00)) {

            /* Get the marker handler from the marker ID*/
            l_marker_handler = opj_j2k_get_marker_handler(l_unknown_marker);

            if (!(p_j2k->m_specific_param.m_decoder.m_state & l_marker_handler->states)) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Marker is not compliant with its position\n");
                return OPJ_FALSE;
            } else {
                if (l_marker_handler->id != J2K_MS_UNK) {
                    /* Add the marker to the codestream index*/
                    if (l_marker_handler->id != J2K_MS_SOT) {
                        OPJ_BOOL res = opj_j2k_add_mhmarker(p_j2k->cstr_index, J2K_MS_UNK,
                                                            (OPJ_UINT32) opj_stream_tell(p_stream) - l_size_unk,
                                                            l_size_unk);
                        if (res == OPJ_FALSE) {
                            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to add mh marker\n");
                            return OPJ_FALSE;
                        }
                    }
                    break; /* next marker is known and well located */
                } else {
                    l_size_unk += 2;
                }
            }
        }
    }

    *output_marker = l_marker_handler->id ;

    return OPJ_TRUE;
}