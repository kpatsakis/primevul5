static void opj_j2k_copy_tile_quantization_parameters(opj_j2k_t *p_j2k)
{
    OPJ_UINT32 i;
    opj_cp_t *l_cp = NULL;
    opj_tcp_t *l_tcp = NULL;
    opj_tccp_t *l_ref_tccp = NULL;
    opj_tccp_t *l_copied_tccp = NULL;
    OPJ_UINT32 l_size;

    /* preconditions */
    assert(p_j2k != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH ?
            &l_cp->tcps[p_j2k->m_current_tile_number] :
            p_j2k->m_specific_param.m_decoder.m_default_tcp;

    l_ref_tccp = &l_tcp->tccps[0];
    l_copied_tccp = l_ref_tccp + 1;
    l_size = OPJ_J2K_MAXBANDS * sizeof(opj_stepsize_t);

    for (i = 1; i < p_j2k->m_private_image->numcomps; ++i) {
        l_copied_tccp->qntsty = l_ref_tccp->qntsty;
        l_copied_tccp->numgbits = l_ref_tccp->numgbits;
        memcpy(l_copied_tccp->stepsizes, l_ref_tccp->stepsizes, l_size);
        ++l_copied_tccp;
    }
}