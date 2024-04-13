static OPJ_BOOL opj_j2k_write_SQcd_SQcc(opj_j2k_t *p_j2k,
                                        OPJ_UINT32 p_tile_no,
                                        OPJ_UINT32 p_comp_no,
                                        OPJ_BYTE * p_data,
                                        OPJ_UINT32 * p_header_size,
                                        struct opj_event_mgr * p_manager)
{
    OPJ_UINT32 l_header_size;
    OPJ_UINT32 l_band_no, l_num_bands;
    OPJ_UINT32 l_expn, l_mant;

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
    assert(p_tile_no < l_cp->tw * l_cp->th);
    assert(p_comp_no < p_j2k->m_private_image->numcomps);

    l_num_bands = (l_tccp->qntsty == J2K_CCP_QNTSTY_SIQNT) ? 1 :
                  (l_tccp->numresolutions * 3 - 2);

    if (l_tccp->qntsty == J2K_CCP_QNTSTY_NOQNT)  {
        l_header_size = 1 + l_num_bands;

        if (*p_header_size < l_header_size) {
            opj_event_msg(p_manager, EVT_ERROR, "Error writing SQcd SQcc element\n");
            return OPJ_FALSE;
        }

        opj_write_bytes(p_data, l_tccp->qntsty + (l_tccp->numgbits << 5),
                        1);   /* Sqcx */
        ++p_data;

        for (l_band_no = 0; l_band_no < l_num_bands; ++l_band_no) {
            l_expn = (OPJ_UINT32)l_tccp->stepsizes[l_band_no].expn;
            opj_write_bytes(p_data, l_expn << 3, 1);        /* SPqcx_i */
            ++p_data;
        }
    } else {
        l_header_size = 1 + 2 * l_num_bands;

        if (*p_header_size < l_header_size) {
            opj_event_msg(p_manager, EVT_ERROR, "Error writing SQcd SQcc element\n");
            return OPJ_FALSE;
        }

        opj_write_bytes(p_data, l_tccp->qntsty + (l_tccp->numgbits << 5),
                        1);   /* Sqcx */
        ++p_data;

        for (l_band_no = 0; l_band_no < l_num_bands; ++l_band_no) {
            l_expn = (OPJ_UINT32)l_tccp->stepsizes[l_band_no].expn;
            l_mant = (OPJ_UINT32)l_tccp->stepsizes[l_band_no].mant;

            opj_write_bytes(p_data, (l_expn << 11) + l_mant, 2);    /* SPqcx_i */
            p_data += 2;
        }
    }

    *p_header_size = *p_header_size - l_header_size;

    return OPJ_TRUE;
}