static OPJ_BOOL opj_tcd_t1_encode(opj_tcd_t *p_tcd)
{
    const OPJ_FLOAT64 * l_mct_norms;
    OPJ_UINT32 l_mct_numcomps = 0U;
    opj_tcp_t * l_tcp = p_tcd->tcp;

    if (l_tcp->mct == 1) {
        l_mct_numcomps = 3U;
        /* irreversible encoding */
        if (l_tcp->tccps->qmfbid == 0) {
            l_mct_norms = opj_mct_get_mct_norms_real();
        } else {
            l_mct_norms = opj_mct_get_mct_norms();
        }
    } else {
        l_mct_numcomps = p_tcd->image->numcomps;
        l_mct_norms = (const OPJ_FLOAT64 *)(l_tcp->mct_norms);
    }

    return opj_t1_encode_cblks(p_tcd,
                               p_tcd->tcd_image->tiles, l_tcp, l_mct_norms,
                               l_mct_numcomps);

    return OPJ_TRUE;
}