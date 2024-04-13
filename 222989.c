static OPJ_UINT32 opj_j2k_get_SPCod_SPCoc_size(opj_j2k_t *p_j2k,
        OPJ_UINT32 p_tile_no,
        OPJ_UINT32 p_comp_no)
{
    opj_cp_t *l_cp = 00;
    opj_tcp_t *l_tcp = 00;
    opj_tccp_t *l_tccp = 00;

    /* preconditions */
    assert(p_j2k != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = &l_cp->tcps[p_tile_no];
    l_tccp = &l_tcp->tccps[p_comp_no];

    /* preconditions again */
    assert(p_tile_no < (l_cp->tw * l_cp->th));
    assert(p_comp_no < p_j2k->m_private_image->numcomps);

    if (l_tccp->csty & J2K_CCP_CSTY_PRT) {
        return 5 + l_tccp->numresolutions;
    } else {
        return 5;
    }
}