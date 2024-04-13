static OPJ_BOOL opj_j2k_write_all_coc(
    opj_j2k_t *p_j2k,
    struct opj_stream_private *p_stream,
    struct opj_event_mgr * p_manager)
{
    OPJ_UINT32 compno;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    for (compno = 1; compno < p_j2k->m_private_image->numcomps; ++compno) {
        /* cod is first component of first tile */
        if (! opj_j2k_compare_coc(p_j2k, 0, compno)) {
            if (! opj_j2k_write_coc(p_j2k, compno, p_stream, p_manager)) {
                return OPJ_FALSE;
            }
        }
    }

    return OPJ_TRUE;
}