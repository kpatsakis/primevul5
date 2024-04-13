static OPJ_BOOL opj_j2k_compare_coc(opj_j2k_t *p_j2k,
                                    OPJ_UINT32 p_first_comp_no, OPJ_UINT32 p_second_comp_no)
{
    opj_cp_t *l_cp = NULL;
    opj_tcp_t *l_tcp = NULL;

    /* preconditions */
    assert(p_j2k != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = &l_cp->tcps[p_j2k->m_current_tile_number];

    if (l_tcp->tccps[p_first_comp_no].csty != l_tcp->tccps[p_second_comp_no].csty) {
        return OPJ_FALSE;
    }


    return opj_j2k_compare_SPCod_SPCoc(p_j2k, p_j2k->m_current_tile_number,
                                       p_first_comp_no, p_second_comp_no);
}