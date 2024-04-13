static void opj_j2k_copy_tile_component_parameters(opj_j2k_t *p_j2k)
{
    /* loop */
    OPJ_UINT32 i;
    opj_cp_t *l_cp = NULL;
    opj_tcp_t *l_tcp = NULL;
    opj_tccp_t *l_ref_tccp = NULL, *l_copied_tccp = NULL;
    OPJ_UINT32 l_prc_size;

    /* preconditions */
    assert(p_j2k != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = (p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH)
            ?
            &l_cp->tcps[p_j2k->m_current_tile_number] :
            p_j2k->m_specific_param.m_decoder.m_default_tcp;

    l_ref_tccp = &l_tcp->tccps[0];
    l_copied_tccp = l_ref_tccp + 1;
    l_prc_size = l_ref_tccp->numresolutions * (OPJ_UINT32)sizeof(OPJ_UINT32);

    for (i = 1; i < p_j2k->m_private_image->numcomps; ++i) {
        l_copied_tccp->numresolutions = l_ref_tccp->numresolutions;
        l_copied_tccp->cblkw = l_ref_tccp->cblkw;
        l_copied_tccp->cblkh = l_ref_tccp->cblkh;
        l_copied_tccp->cblksty = l_ref_tccp->cblksty;
        l_copied_tccp->qmfbid = l_ref_tccp->qmfbid;
        memcpy(l_copied_tccp->prcw, l_ref_tccp->prcw, l_prc_size);
        memcpy(l_copied_tccp->prch, l_ref_tccp->prch, l_prc_size);
        ++l_copied_tccp;
    }
}