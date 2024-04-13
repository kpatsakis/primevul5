OPJ_BOOL opj_j2k_end_compress(opj_j2k_t *p_j2k,
                              opj_stream_private_t *p_stream,
                              opj_event_mgr_t * p_manager)
{
    /* customization of the encoding */
    if (! opj_j2k_setup_end_compress(p_j2k, p_manager)) {
        return OPJ_FALSE;
    }

    if (! opj_j2k_exec(p_j2k, p_j2k->m_procedure_list, p_stream, p_manager)) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}