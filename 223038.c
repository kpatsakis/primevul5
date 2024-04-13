static OPJ_BOOL opj_j2k_read_qcd(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    if (! opj_j2k_read_SQcd_SQcc(p_j2k, 0, p_header_data, &p_header_size,
                                 p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading QCD marker\n");
        return OPJ_FALSE;
    }

    if (p_header_size != 0) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading QCD marker\n");
        return OPJ_FALSE;
    }

    /* Apply the quantization parameters to other components of the current tile or the m_default_tcp */
    opj_j2k_copy_tile_quantization_parameters(p_j2k);

    return OPJ_TRUE;
}