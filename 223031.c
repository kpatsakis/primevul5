static OPJ_BOOL opj_j2k_compare_SPCod_SPCoc(opj_j2k_t *p_j2k,
        OPJ_UINT32 p_tile_no, OPJ_UINT32 p_first_comp_no, OPJ_UINT32 p_second_comp_no)
{
    OPJ_UINT32 i;
    opj_cp_t *l_cp = NULL;
    opj_tcp_t *l_tcp = NULL;
    opj_tccp_t *l_tccp0 = NULL;
    opj_tccp_t *l_tccp1 = NULL;

    /* preconditions */
    assert(p_j2k != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = &l_cp->tcps[p_tile_no];
    l_tccp0 = &l_tcp->tccps[p_first_comp_no];
    l_tccp1 = &l_tcp->tccps[p_second_comp_no];

    if (l_tccp0->numresolutions != l_tccp1->numresolutions) {
        return OPJ_FALSE;
    }
    if (l_tccp0->cblkw != l_tccp1->cblkw) {
        return OPJ_FALSE;
    }
    if (l_tccp0->cblkh != l_tccp1->cblkh) {
        return OPJ_FALSE;
    }
    if (l_tccp0->cblksty != l_tccp1->cblksty) {
        return OPJ_FALSE;
    }
    if (l_tccp0->qmfbid != l_tccp1->qmfbid) {
        return OPJ_FALSE;
    }
    if ((l_tccp0->csty & J2K_CCP_CSTY_PRT) != (l_tccp1->csty & J2K_CCP_CSTY_PRT)) {
        return OPJ_FALSE;
    }

    for (i = 0U; i < l_tccp0->numresolutions; ++i) {
        if (l_tccp0->prcw[i] != l_tccp1->prcw[i]) {
            return OPJ_FALSE;
        }
        if (l_tccp0->prch[i] != l_tccp1->prch[i]) {
            return OPJ_FALSE;
        }
    }
    return OPJ_TRUE;
}