static OPJ_BOOL opj_j2k_write_regions(opj_j2k_t *p_j2k,
                                      struct opj_stream_private *p_stream,
                                      struct opj_event_mgr * p_manager)
{
    OPJ_UINT32 compno;
    const opj_tccp_t *l_tccp = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_tccp = p_j2k->m_cp.tcps->tccps;

    for (compno = 0; compno < p_j2k->m_private_image->numcomps; ++compno)  {
        if (l_tccp->roishift) {

            if (! opj_j2k_write_rgn(p_j2k, 0, compno, p_j2k->m_private_image->numcomps,
                                    p_stream, p_manager)) {
                return OPJ_FALSE;
            }
        }

        ++l_tccp;
    }

    return OPJ_TRUE;
}