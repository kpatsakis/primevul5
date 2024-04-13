static OPJ_BOOL opj_j2k_get_end_header(opj_j2k_t *p_j2k,
                                       struct opj_stream_private *p_stream,
                                       struct opj_event_mgr * p_manager)
{
    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    OPJ_UNUSED(p_manager);

    p_j2k->cstr_index->main_head_end = opj_stream_tell(p_stream);

    return OPJ_TRUE;
}