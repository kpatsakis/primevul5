static OPJ_BOOL opj_j2k_init_info(opj_j2k_t *p_j2k,
                                  struct opj_stream_private *p_stream,
                                  struct opj_event_mgr * p_manager)
{
    opj_codestream_info_t * l_cstr_info = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);
    (void)l_cstr_info;

    OPJ_UNUSED(p_stream);

    /* TODO mergeV2: check this part which use cstr_info */
    /*l_cstr_info = p_j2k->cstr_info;

    if (l_cstr_info)  {
            OPJ_UINT32 compno;
            l_cstr_info->tile = (opj_tile_info_t *) opj_malloc(p_j2k->m_cp.tw * p_j2k->m_cp.th * sizeof(opj_tile_info_t));

            l_cstr_info->image_w = p_j2k->m_image->x1 - p_j2k->m_image->x0;
            l_cstr_info->image_h = p_j2k->m_image->y1 - p_j2k->m_image->y0;

            l_cstr_info->prog = (&p_j2k->m_cp.tcps[0])->prg;

            l_cstr_info->tw = p_j2k->m_cp.tw;
            l_cstr_info->th = p_j2k->m_cp.th;

            l_cstr_info->tile_x = p_j2k->m_cp.tdx;*/        /* new version parser */
    /*l_cstr_info->tile_y = p_j2k->m_cp.tdy;*/      /* new version parser */
    /*l_cstr_info->tile_Ox = p_j2k->m_cp.tx0;*/     /* new version parser */
    /*l_cstr_info->tile_Oy = p_j2k->m_cp.ty0;*/     /* new version parser */

    /*l_cstr_info->numcomps = p_j2k->m_image->numcomps;

    l_cstr_info->numlayers = (&p_j2k->m_cp.tcps[0])->numlayers;

    l_cstr_info->numdecompos = (OPJ_INT32*) opj_malloc(p_j2k->m_image->numcomps * sizeof(OPJ_INT32));

    for (compno=0; compno < p_j2k->m_image->numcomps; compno++) {
            l_cstr_info->numdecompos[compno] = (&p_j2k->m_cp.tcps[0])->tccps->numresolutions - 1;
    }

    l_cstr_info->D_max = 0.0;       */      /* ADD Marcela */

    /*l_cstr_info->main_head_start = opj_stream_tell(p_stream);*/ /* position of SOC */

    /*l_cstr_info->maxmarknum = 100;
    l_cstr_info->marker = (opj_marker_info_t *) opj_malloc(l_cstr_info->maxmarknum * sizeof(opj_marker_info_t));
    l_cstr_info->marknum = 0;
    }*/

    return opj_j2k_calculate_tp(p_j2k, &(p_j2k->m_cp),
                                &p_j2k->m_specific_param.m_encoder.m_total_tile_parts, p_j2k->m_private_image,
                                p_manager);
}