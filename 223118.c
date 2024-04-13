static OPJ_BOOL opj_j2k_exec(opj_j2k_t * p_j2k,
                             opj_procedure_list_t * p_procedure_list,
                             opj_stream_private_t *p_stream,
                             opj_event_mgr_t * p_manager)
{
    OPJ_BOOL(** l_procedure)(opj_j2k_t *, opj_stream_private_t *,
                             opj_event_mgr_t *) = 00;
    OPJ_BOOL l_result = OPJ_TRUE;
    OPJ_UINT32 l_nb_proc, i;

    /* preconditions*/
    assert(p_procedure_list != 00);
    assert(p_j2k != 00);
    assert(p_stream != 00);
    assert(p_manager != 00);

    l_nb_proc = opj_procedure_list_get_nb_procedures(p_procedure_list);
    l_procedure = (OPJ_BOOL(**)(opj_j2k_t *, opj_stream_private_t *,
                                opj_event_mgr_t *)) opj_procedure_list_get_first_procedure(p_procedure_list);

    for (i = 0; i < l_nb_proc; ++i) {
        l_result = l_result && ((*l_procedure)(p_j2k, p_stream, p_manager));
        ++l_procedure;
    }

    /* and clear the procedure list at the end.*/
    opj_procedure_list_clear(p_procedure_list);
    return l_result;
}