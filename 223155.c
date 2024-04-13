static OPJ_BOOL opj_tcd_mct_encode(opj_tcd_t *p_tcd)
{
    opj_tcd_tile_t * l_tile = p_tcd->tcd_image->tiles;
    opj_tcd_tilecomp_t * l_tile_comp = p_tcd->tcd_image->tiles->comps;
    OPJ_SIZE_T samples = (OPJ_SIZE_T)(l_tile_comp->x1 - l_tile_comp->x0) *
                         (OPJ_SIZE_T)(l_tile_comp->y1 - l_tile_comp->y0);
    OPJ_UINT32 i;
    OPJ_BYTE ** l_data = 00;
    opj_tcp_t * l_tcp = p_tcd->tcp;

    if (!p_tcd->tcp->mct) {
        return OPJ_TRUE;
    }

    if (p_tcd->tcp->mct == 2) {
        if (! p_tcd->tcp->m_mct_coding_matrix) {
            return OPJ_TRUE;
        }

        l_data = (OPJ_BYTE **) opj_malloc(l_tile->numcomps * sizeof(OPJ_BYTE*));
        if (! l_data) {
            return OPJ_FALSE;
        }

        for (i = 0; i < l_tile->numcomps; ++i) {
            l_data[i] = (OPJ_BYTE*) l_tile_comp->data;
            ++l_tile_comp;
        }

        if (! opj_mct_encode_custom(/* MCT data */
                    (OPJ_BYTE*) p_tcd->tcp->m_mct_coding_matrix,
                    /* size of components */
                    samples,
                    /* components */
                    l_data,
                    /* nb of components (i.e. size of pData) */
                    l_tile->numcomps,
                    /* tells if the data is signed */
                    p_tcd->image->comps->sgnd)) {
            opj_free(l_data);
            return OPJ_FALSE;
        }

        opj_free(l_data);
    } else if (l_tcp->tccps->qmfbid == 0) {
        opj_mct_encode_real(
            (OPJ_FLOAT32*)l_tile->comps[0].data,
            (OPJ_FLOAT32*)l_tile->comps[1].data,
            (OPJ_FLOAT32*)l_tile->comps[2].data,
            samples);
    } else {
        opj_mct_encode(l_tile->comps[0].data, l_tile->comps[1].data,
                       l_tile->comps[2].data, samples);
    }

    return OPJ_TRUE;
}