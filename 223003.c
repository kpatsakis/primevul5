static OPJ_BOOL opj_j2k_write_soc(opj_j2k_t *p_j2k,
                                  opj_stream_private_t *p_stream,
                                  opj_event_mgr_t * p_manager)
{
    /* 2 bytes will be written */
    OPJ_BYTE * l_start_stream = 00;

    /* preconditions */
    assert(p_stream != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_start_stream = p_j2k->m_specific_param.m_encoder.m_header_tile_data;

    /* write SOC identifier */
    opj_write_bytes(l_start_stream, J2K_MS_SOC, 2);

    if (opj_stream_write_data(p_stream, l_start_stream, 2, p_manager) != 2) {
        return OPJ_FALSE;
    }

    /* UniPG>> */
#ifdef USE_JPWL
    /* update markers struct */
    /*
            OPJ_BOOL res = j2k_add_marker(p_j2k->cstr_info, J2K_MS_SOC, p_stream_tell(p_stream) - 2, 2);
    */
    assert(0 && "TODO");
#endif /* USE_JPWL */
    /* <<UniPG */

    return OPJ_TRUE;
}