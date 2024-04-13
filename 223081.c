static OPJ_BOOL opj_j2k_compare_SQcd_SQcc(opj_j2k_t *p_j2k,
        OPJ_UINT32 p_tile_no, OPJ_UINT32 p_first_comp_no, OPJ_UINT32 p_second_comp_no)
{
    opj_cp_t *l_cp = NULL;
    opj_tcp_t *l_tcp = NULL;
    opj_tccp_t *l_tccp0 = NULL;
    opj_tccp_t *l_tccp1 = NULL;
    OPJ_UINT32 l_band_no, l_num_bands;

    /* preconditions */
    assert(p_j2k != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = &l_cp->tcps[p_tile_no];
    l_tccp0 = &l_tcp->tccps[p_first_comp_no];
    l_tccp1 = &l_tcp->tccps[p_second_comp_no];

    if (l_tccp0->qntsty != l_tccp1->qntsty) {
        return OPJ_FALSE;
    }
    if (l_tccp0->numgbits != l_tccp1->numgbits) {
        return OPJ_FALSE;
    }
    if (l_tccp0->qntsty == J2K_CCP_QNTSTY_SIQNT) {
        l_num_bands = 1U;
    } else {
        l_num_bands = l_tccp0->numresolutions * 3U - 2U;
        if (l_num_bands != (l_tccp1->numresolutions * 3U - 2U)) {
            return OPJ_FALSE;
        }
    }

    for (l_band_no = 0; l_band_no < l_num_bands; ++l_band_no) {
        if (l_tccp0->stepsizes[l_band_no].expn != l_tccp1->stepsizes[l_band_no].expn) {
            return OPJ_FALSE;
        }
    }
    if (l_tccp0->qntsty != J2K_CCP_QNTSTY_NOQNT) {
        for (l_band_no = 0; l_band_no < l_num_bands; ++l_band_no) {
            if (l_tccp0->stepsizes[l_band_no].mant != l_tccp1->stepsizes[l_band_no].mant) {
                return OPJ_FALSE;
            }
        }
    }
    return OPJ_TRUE;
}