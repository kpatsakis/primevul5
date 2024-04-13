static OPJ_BOOL opj_j2k_mct_validation(opj_j2k_t * p_j2k,
                                       opj_stream_private_t *p_stream,
                                       opj_event_mgr_t * p_manager)
{
    OPJ_BOOL l_is_valid = OPJ_TRUE;
    OPJ_UINT32 i, j;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_stream != 00);
    assert(p_manager != 00);

    OPJ_UNUSED(p_stream);
    OPJ_UNUSED(p_manager);

    if ((p_j2k->m_cp.rsiz & 0x8200) == 0x8200) {
        OPJ_UINT32 l_nb_tiles = p_j2k->m_cp.th * p_j2k->m_cp.tw;
        opj_tcp_t * l_tcp = p_j2k->m_cp.tcps;

        for (i = 0; i < l_nb_tiles; ++i) {
            if (l_tcp->mct == 2) {
                opj_tccp_t * l_tccp = l_tcp->tccps;
                l_is_valid &= (l_tcp->m_mct_coding_matrix != 00);

                for (j = 0; j < p_j2k->m_private_image->numcomps; ++j) {
                    l_is_valid &= !(l_tccp->qmfbid & 1);
                    ++l_tccp;
                }
            }
            ++l_tcp;
        }
    }

    return l_is_valid;
}