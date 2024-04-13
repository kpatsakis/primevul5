OPJ_BOOL opj_tcd_init(opj_tcd_t *p_tcd,
                      opj_image_t * p_image,
                      opj_cp_t * p_cp,
                      opj_thread_pool_t* p_tp)
{
    p_tcd->image = p_image;
    p_tcd->cp = p_cp;

    p_tcd->tcd_image->tiles = (opj_tcd_tile_t *) opj_calloc(1,
                              sizeof(opj_tcd_tile_t));
    if (! p_tcd->tcd_image->tiles) {
        return OPJ_FALSE;
    }

    p_tcd->tcd_image->tiles->comps = (opj_tcd_tilecomp_t *) opj_calloc(
                                         p_image->numcomps, sizeof(opj_tcd_tilecomp_t));
    if (! p_tcd->tcd_image->tiles->comps) {
        return OPJ_FALSE;
    }

    p_tcd->tcd_image->tiles->numcomps = p_image->numcomps;
    p_tcd->tp_pos = p_cp->m_specific_param.m_enc.m_tp_pos;
    p_tcd->thread_pool = p_tp;

    return OPJ_TRUE;
}