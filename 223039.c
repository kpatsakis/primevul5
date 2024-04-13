static OPJ_BOOL opj_j2k_read_crg(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 l_nb_comp;
    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    OPJ_UNUSED(p_header_data);

    l_nb_comp = p_j2k->m_private_image->numcomps;

    if (p_header_size != l_nb_comp * 4) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading CRG marker\n");
        return OPJ_FALSE;
    }
    /* Do not care of this at the moment since only local variables are set here */
    /*
    for
            (i = 0; i < l_nb_comp; ++i)
    {
            opj_read_bytes(p_header_data,&l_Xcrg_i,2);                              // Xcrg_i
            p_header_data+=2;
            opj_read_bytes(p_header_data,&l_Ycrg_i,2);                              // Xcrg_i
            p_header_data+=2;
    }
    */
    return OPJ_TRUE;
}