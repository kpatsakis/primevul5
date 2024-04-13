static OPJ_BOOL opj_j2k_read_tlm(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 l_Ztlm, l_Stlm, l_ST, l_SP, l_tot_num_tp_remaining, l_quotient,
               l_Ptlm_size;
    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    OPJ_UNUSED(p_j2k);

    if (p_header_size < 2) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading TLM marker\n");
        return OPJ_FALSE;
    }
    p_header_size -= 2;

    opj_read_bytes(p_header_data, &l_Ztlm,
                   1);                              /* Ztlm */
    ++p_header_data;
    opj_read_bytes(p_header_data, &l_Stlm,
                   1);                              /* Stlm */
    ++p_header_data;

    l_ST = ((l_Stlm >> 4) & 0x3);
    l_SP = (l_Stlm >> 6) & 0x1;

    l_Ptlm_size = (l_SP + 1) * 2;
    l_quotient = l_Ptlm_size + l_ST;

    l_tot_num_tp_remaining = p_header_size % l_quotient;

    if (l_tot_num_tp_remaining != 0) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading TLM marker\n");
        return OPJ_FALSE;
    }
    /* FIXME Do not care of this at the moment since only local variables are set here */
    /*
    for
            (i = 0; i < l_tot_num_tp; ++i)
    {
            opj_read_bytes(p_header_data,&l_Ttlm_i,l_ST);                           // Ttlm_i
            p_header_data += l_ST;
            opj_read_bytes(p_header_data,&l_Ptlm_i,l_Ptlm_size);            // Ptlm_i
            p_header_data += l_Ptlm_size;
    }*/
    return OPJ_TRUE;
}