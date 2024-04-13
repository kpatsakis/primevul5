static OPJ_BOOL opj_j2k_setup_decoding(opj_j2k_t *p_j2k,
                                       opj_event_mgr_t * p_manager)
{
    /* preconditions*/
    assert(p_j2k != 00);
    assert(p_manager != 00);

    if (! opj_procedure_list_add_procedure(p_j2k->m_procedure_list,
                                           (opj_procedure)opj_j2k_decode_tiles, p_manager)) {
        return OPJ_FALSE;
    }
    /* DEVELOPER CORNER, add your custom procedures */

    return OPJ_TRUE;
}