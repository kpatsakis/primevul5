static OPJ_BOOL opj_j2k_create_tcd(opj_j2k_t *p_j2k,
                                   opj_stream_private_t *p_stream,
                                   opj_event_mgr_t * p_manager
                                  )
{
    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    OPJ_UNUSED(p_stream);

    p_j2k->m_tcd = opj_tcd_create(OPJ_FALSE);

    if (! p_j2k->m_tcd) {
        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to create Tile Coder\n");
        return OPJ_FALSE;
    }

    if (!opj_tcd_init(p_j2k->m_tcd, p_j2k->m_private_image, &p_j2k->m_cp,
                      p_j2k->m_tp)) {
        opj_tcd_destroy(p_j2k->m_tcd);
        p_j2k->m_tcd = 00;
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}