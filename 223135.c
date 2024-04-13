static OPJ_BOOL opj_tcd_dwt_encode(opj_tcd_t *p_tcd)
{
    opj_tcd_tile_t * l_tile = p_tcd->tcd_image->tiles;
    opj_tcd_tilecomp_t * l_tile_comp = p_tcd->tcd_image->tiles->comps;
    opj_tccp_t * l_tccp = p_tcd->tcp->tccps;
    OPJ_UINT32 compno;

    for (compno = 0; compno < l_tile->numcomps; ++compno) {
        if (l_tccp->qmfbid == 1) {
            if (! opj_dwt_encode(p_tcd, l_tile_comp)) {
                return OPJ_FALSE;
            }
        } else if (l_tccp->qmfbid == 0) {
            if (! opj_dwt_encode_real(p_tcd, l_tile_comp)) {
                return OPJ_FALSE;
            }
        }

        ++l_tile_comp;
        ++l_tccp;
    }

    return OPJ_TRUE;
}