static OPJ_BOOL opj_j2k_read_plt(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 l_Zplt, l_tmp, l_packet_len = 0, i;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    OPJ_UNUSED(p_j2k);

    if (p_header_size < 1) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading PLT marker\n");
        return OPJ_FALSE;
    }

    opj_read_bytes(p_header_data, &l_Zplt, 1);              /* Zplt */
    ++p_header_data;
    --p_header_size;

    for (i = 0; i < p_header_size; ++i) {
        opj_read_bytes(p_header_data, &l_tmp, 1);       /* Iplt_ij */
        ++p_header_data;
        /* take only the last seven bytes */
        l_packet_len |= (l_tmp & 0x7f);
        if (l_tmp & 0x80) {
            l_packet_len <<= 7;
        } else {
            /* store packet length and proceed to next packet */
            l_packet_len = 0;
        }
    }

    if (l_packet_len != 0) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading PLT marker\n");
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}