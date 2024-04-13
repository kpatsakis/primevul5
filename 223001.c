opj_codestream_info_v2_t* j2k_get_cstr_info(opj_j2k_t* p_j2k)
{
    OPJ_UINT32 compno;
    OPJ_UINT32 numcomps = p_j2k->m_private_image->numcomps;
    opj_tcp_t *l_default_tile;
    opj_codestream_info_v2_t* cstr_info = (opj_codestream_info_v2_t*) opj_calloc(1,
                                          sizeof(opj_codestream_info_v2_t));
    if (!cstr_info) {
        return NULL;
    }

    cstr_info->nbcomps = p_j2k->m_private_image->numcomps;

    cstr_info->tx0 = p_j2k->m_cp.tx0;
    cstr_info->ty0 = p_j2k->m_cp.ty0;
    cstr_info->tdx = p_j2k->m_cp.tdx;
    cstr_info->tdy = p_j2k->m_cp.tdy;
    cstr_info->tw = p_j2k->m_cp.tw;
    cstr_info->th = p_j2k->m_cp.th;

    cstr_info->tile_info = NULL; /* Not fill from the main header*/

    l_default_tile = p_j2k->m_specific_param.m_decoder.m_default_tcp;

    cstr_info->m_default_tile_info.csty = l_default_tile->csty;
    cstr_info->m_default_tile_info.prg = l_default_tile->prg;
    cstr_info->m_default_tile_info.numlayers = l_default_tile->numlayers;
    cstr_info->m_default_tile_info.mct = l_default_tile->mct;

    cstr_info->m_default_tile_info.tccp_info = (opj_tccp_info_t*) opj_calloc(
                cstr_info->nbcomps, sizeof(opj_tccp_info_t));
    if (!cstr_info->m_default_tile_info.tccp_info) {
        opj_destroy_cstr_info(&cstr_info);
        return NULL;
    }

    for (compno = 0; compno < numcomps; compno++) {
        opj_tccp_t *l_tccp = &(l_default_tile->tccps[compno]);
        opj_tccp_info_t *l_tccp_info = &
                                       (cstr_info->m_default_tile_info.tccp_info[compno]);
        OPJ_INT32 bandno, numbands;

        /* coding style*/
        l_tccp_info->csty = l_tccp->csty;
        l_tccp_info->numresolutions = l_tccp->numresolutions;
        l_tccp_info->cblkw = l_tccp->cblkw;
        l_tccp_info->cblkh = l_tccp->cblkh;
        l_tccp_info->cblksty = l_tccp->cblksty;
        l_tccp_info->qmfbid = l_tccp->qmfbid;
        if (l_tccp->numresolutions < OPJ_J2K_MAXRLVLS) {
            memcpy(l_tccp_info->prch, l_tccp->prch, l_tccp->numresolutions);
            memcpy(l_tccp_info->prcw, l_tccp->prcw, l_tccp->numresolutions);
        }

        /* quantization style*/
        l_tccp_info->qntsty = l_tccp->qntsty;
        l_tccp_info->numgbits = l_tccp->numgbits;

        numbands = (l_tccp->qntsty == J2K_CCP_QNTSTY_SIQNT) ? 1 :
                   (OPJ_INT32)l_tccp->numresolutions * 3 - 2;
        if (numbands < OPJ_J2K_MAXBANDS) {
            for (bandno = 0; bandno < numbands; bandno++) {
                l_tccp_info->stepsizes_mant[bandno] = (OPJ_UINT32)
                                                      l_tccp->stepsizes[bandno].mant;
                l_tccp_info->stepsizes_expn[bandno] = (OPJ_UINT32)
                                                      l_tccp->stepsizes[bandno].expn;
            }
        }

        /* RGN value*/
        l_tccp_info->roishift = l_tccp->roishift;
    }

    return cstr_info;
}