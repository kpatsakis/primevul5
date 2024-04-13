static OPJ_BOOL opj_j2k_read_plm(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    OPJ_UNUSED(p_j2k);
    OPJ_UNUSED(p_header_data);

    if (p_header_size < 1) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading PLM marker\n");
        return OPJ_FALSE;
    }
    /* Do not care of this at the moment since only local variables are set here */
    /*
    opj_read_bytes(p_header_data,&l_Zplm,1);                                        // Zplm
    ++p_header_data;
    --p_header_size;

    while
            (p_header_size > 0)
    {
            opj_read_bytes(p_header_data,&l_Nplm,1);                                // Nplm
            ++p_header_data;
            p_header_size -= (1+l_Nplm);
            if
                    (p_header_size < 0)
            {
                    opj_event_msg(p_manager, EVT_ERROR, "Error reading PLM marker\n");
                    return false;
            }
            for
                    (i = 0; i < l_Nplm; ++i)
            {
                    opj_read_bytes(p_header_data,&l_tmp,1);                         // Iplm_ij
                    ++p_header_data;
                    // take only the last seven bytes
                    l_packet_len |= (l_tmp & 0x7f);
                    if
                            (l_tmp & 0x80)
                    {
                            l_packet_len <<= 7;
                    }
                    else
                    {
            // store packet length and proceed to next packet
                            l_packet_len = 0;
                    }
            }
            if
                    (l_packet_len != 0)
            {
                    opj_event_msg(p_manager, EVT_ERROR, "Error reading PLM marker\n");
                    return false;
            }
    }
    */
    return OPJ_TRUE;
}