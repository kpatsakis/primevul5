opj_tcd_t* opj_tcd_create(OPJ_BOOL p_is_decoder)
{
    opj_tcd_t *l_tcd = 00;

    /* create the tcd structure */
    l_tcd = (opj_tcd_t*) opj_calloc(1, sizeof(opj_tcd_t));
    if (!l_tcd) {
        return 00;
    }

    l_tcd->m_is_decoder = p_is_decoder ? 1 : 0;

    l_tcd->tcd_image = (opj_tcd_image_t*)opj_calloc(1, sizeof(opj_tcd_image_t));
    if (!l_tcd->tcd_image) {
        opj_free(l_tcd);
        return 00;
    }

    return l_tcd;
}