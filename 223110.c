static OPJ_BOOL opj_j2k_setup_header_reading(opj_j2k_t *p_j2k,
        opj_event_mgr_t * p_manager)
{
    /* preconditions*/
    assert(p_j2k != 00);
    assert(p_manager != 00);

    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_read_header_procedure, p_manager)) {
        return OPJ_FALSE;
    }

    /* DEVELOPER CORNER, add your custom procedures */
    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_copy_default_tcp_and_create_tcd, p_manager))  {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}