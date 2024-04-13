static OPJ_BOOL opj_j2k_write_SPCod_SPCoc(opj_j2k_t *p_j2k,
        OPJ_UINT32 p_tile_no,
        OPJ_UINT32 p_comp_no,
        OPJ_BYTE * p_data,
        OPJ_UINT32 * p_header_size,
        struct opj_event_mgr * p_manager)
{
    OPJ_UINT32 i;
    opj_cp_t *l_cp = 00;
    opj_tcp_t *l_tcp = 00;
    opj_tccp_t *l_tccp = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_header_size != 00);
    assert(p_manager != 00);
    assert(p_data != 00);

    l_cp = &(p_j2k->m_cp);
    l_tcp = &l_cp->tcps[p_tile_no];
    l_tccp = &l_tcp->tccps[p_comp_no];

    /* preconditions again */
    assert(p_tile_no < (l_cp->tw * l_cp->th));
    assert(p_comp_no < (p_j2k->m_private_image->numcomps));

    if (*p_header_size < 5) {
        opj_event_msg(p_manager, EVT_ERROR, "Error writing SPCod SPCoc element\n");
        return OPJ_FALSE;
    }

    opj_write_bytes(p_data, l_tccp->numresolutions - 1, 1); /* SPcoc (D) */
    ++p_data;

    opj_write_bytes(p_data, l_tccp->cblkw - 2, 1);                  /* SPcoc (E) */
    ++p_data;

    opj_write_bytes(p_data, l_tccp->cblkh - 2, 1);                  /* SPcoc (F) */
    ++p_data;

    opj_write_bytes(p_data, l_tccp->cblksty,
                    1);                            /* SPcoc (G) */
    ++p_data;

    opj_write_bytes(p_data, l_tccp->qmfbid,
                    1);                             /* SPcoc (H) */
    ++p_data;

    *p_header_size = *p_header_size - 5;

    if (l_tccp->csty & J2K_CCP_CSTY_PRT) {

        if (*p_header_size < l_tccp->numresolutions) {
            opj_event_msg(p_manager, EVT_ERROR, "Error writing SPCod SPCoc element\n");
            return OPJ_FALSE;
        }

        for (i = 0; i < l_tccp->numresolutions; ++i) {
            opj_write_bytes(p_data, l_tccp->prcw[i] + (l_tccp->prch[i] << 4),
                            1);   /* SPcoc (I_i) */
            ++p_data;
        }

        *p_header_size = *p_header_size - l_tccp->numresolutions;
    }

    return OPJ_TRUE;
}