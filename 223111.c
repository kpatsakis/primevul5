static OPJ_BOOL opj_j2k_read_sod(opj_j2k_t *p_j2k,
                                 opj_stream_private_t *p_stream,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_SIZE_T l_current_read_size;
    opj_codestream_index_t * l_cstr_index = 00;
    OPJ_BYTE ** l_current_data = 00;
    opj_tcp_t * l_tcp = 00;
    OPJ_UINT32 * l_tile_len = 00;
    OPJ_BOOL l_sot_length_pb_detected = OPJ_FALSE;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_tcp = &(p_j2k->m_cp.tcps[p_j2k->m_current_tile_number]);

    if (p_j2k->m_specific_param.m_decoder.m_last_tile_part) {
        /* opj_stream_get_number_byte_left returns OPJ_OFF_T
        // but we are in the last tile part,
        // so its result will fit on OPJ_UINT32 unless we find
        // a file with a single tile part of more than 4 GB...*/
        p_j2k->m_specific_param.m_decoder.m_sot_length = (OPJ_UINT32)(
                    opj_stream_get_number_byte_left(p_stream) - 2);
    } else {
        /* Check to avoid pass the limit of OPJ_UINT32 */
        if (p_j2k->m_specific_param.m_decoder.m_sot_length >= 2) {
            p_j2k->m_specific_param.m_decoder.m_sot_length -= 2;
        } else {
            /* MSD: case commented to support empty SOT marker (PHR data) */
        }
    }

    l_current_data = &(l_tcp->m_data);
    l_tile_len = &l_tcp->m_data_size;

    /* Patch to support new PHR data */
    if (p_j2k->m_specific_param.m_decoder.m_sot_length) {
        /* If we are here, we'll try to read the data after allocation */
        /* Check enough bytes left in stream before allocation */
        if ((OPJ_OFF_T)p_j2k->m_specific_param.m_decoder.m_sot_length >
                opj_stream_get_number_byte_left(p_stream)) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Tile part length size inconsistent with stream length\n");
            return OPJ_FALSE;
        }
        if (p_j2k->m_specific_param.m_decoder.m_sot_length >
                UINT_MAX - OPJ_COMMON_CBLK_DATA_EXTRA) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "p_j2k->m_specific_param.m_decoder.m_sot_length > "
                          "UINT_MAX - OPJ_COMMON_CBLK_DATA_EXTRA");
            return OPJ_FALSE;
        }
        /* Add a margin of OPJ_COMMON_CBLK_DATA_EXTRA to the allocation we */
        /* do so that opj_mqc_init_dec_common() can safely add a synthetic */
        /* 0xFFFF marker. */
        if (! *l_current_data) {
            /* LH: oddly enough, in this path, l_tile_len!=0.
             * TODO: If this was consistent, we could simplify the code to only use realloc(), as realloc(0,...) default to malloc(0,...).
             */
            *l_current_data = (OPJ_BYTE*) opj_malloc(
                                  p_j2k->m_specific_param.m_decoder.m_sot_length + OPJ_COMMON_CBLK_DATA_EXTRA);
        } else {
            OPJ_BYTE *l_new_current_data;
            if (*l_tile_len > UINT_MAX - OPJ_COMMON_CBLK_DATA_EXTRA -
                    p_j2k->m_specific_param.m_decoder.m_sot_length) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "*l_tile_len > UINT_MAX - OPJ_COMMON_CBLK_DATA_EXTRA - "
                              "p_j2k->m_specific_param.m_decoder.m_sot_length");
                return OPJ_FALSE;
            }

            l_new_current_data = (OPJ_BYTE *) opj_realloc(*l_current_data,
                                 *l_tile_len + p_j2k->m_specific_param.m_decoder.m_sot_length +
                                 OPJ_COMMON_CBLK_DATA_EXTRA);
            if (! l_new_current_data) {
                opj_free(*l_current_data);
                /*nothing more is done as l_current_data will be set to null, and just
                  afterward we enter in the error path
                  and the actual tile_len is updated (committed) at the end of the
                  function. */
            }
            *l_current_data = l_new_current_data;
        }

        if (*l_current_data == 00) {
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to decode tile\n");
            return OPJ_FALSE;
        }
    } else {
        l_sot_length_pb_detected = OPJ_TRUE;
    }

    /* Index */
    l_cstr_index = p_j2k->cstr_index;
    if (l_cstr_index) {
        OPJ_OFF_T l_current_pos = opj_stream_tell(p_stream) - 2;

        OPJ_UINT32 l_current_tile_part =
            l_cstr_index->tile_index[p_j2k->m_current_tile_number].current_tpsno;
        l_cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index[l_current_tile_part].end_header
            =
                l_current_pos;
        l_cstr_index->tile_index[p_j2k->m_current_tile_number].tp_index[l_current_tile_part].end_pos
            =
                l_current_pos + p_j2k->m_specific_param.m_decoder.m_sot_length + 2;

        if (OPJ_FALSE == opj_j2k_add_tlmarker(p_j2k->m_current_tile_number,
                                              l_cstr_index,
                                              J2K_MS_SOD,
                                              l_current_pos,
                                              p_j2k->m_specific_param.m_decoder.m_sot_length + 2)) {
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to add tl marker\n");
            return OPJ_FALSE;
        }

        /*l_cstr_index->packno = 0;*/
    }

    /* Patch to support new PHR data */
    if (!l_sot_length_pb_detected) {
        l_current_read_size = opj_stream_read_data(
                                  p_stream,
                                  *l_current_data + *l_tile_len,
                                  p_j2k->m_specific_param.m_decoder.m_sot_length,
                                  p_manager);
    } else {
        l_current_read_size = 0;
    }

    if (l_current_read_size != p_j2k->m_specific_param.m_decoder.m_sot_length) {
        p_j2k->m_specific_param.m_decoder.m_state = J2K_STATE_NEOC;
    } else {
        p_j2k->m_specific_param.m_decoder.m_state = J2K_STATE_TPHSOT;
    }

    *l_tile_len += (OPJ_UINT32)l_current_read_size;

    return OPJ_TRUE;
}