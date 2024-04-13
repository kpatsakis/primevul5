static OPJ_BOOL opj_tcd_dc_level_shift_encode(opj_tcd_t *p_tcd)
{
    OPJ_UINT32 compno;
    opj_tcd_tilecomp_t * l_tile_comp = 00;
    opj_tccp_t * l_tccp = 00;
    opj_image_comp_t * l_img_comp = 00;
    opj_tcd_tile_t * l_tile;
    OPJ_SIZE_T l_nb_elem, i;
    OPJ_INT32 * l_current_ptr;

    l_tile = p_tcd->tcd_image->tiles;
    l_tile_comp = l_tile->comps;
    l_tccp = p_tcd->tcp->tccps;
    l_img_comp = p_tcd->image->comps;

    for (compno = 0; compno < l_tile->numcomps; compno++) {
        l_current_ptr = l_tile_comp->data;
        l_nb_elem = (OPJ_SIZE_T)(l_tile_comp->x1 - l_tile_comp->x0) *
                    (OPJ_SIZE_T)(l_tile_comp->y1 - l_tile_comp->y0);

        if (l_tccp->qmfbid == 1) {
            for (i = 0; i < l_nb_elem; ++i) {
                *l_current_ptr -= l_tccp->m_dc_level_shift ;
                ++l_current_ptr;
            }
        } else {
            for (i = 0; i < l_nb_elem; ++i) {
                *((OPJ_FLOAT32 *) l_current_ptr) = (OPJ_FLOAT32)(*l_current_ptr -
                                                   l_tccp->m_dc_level_shift);
                ++l_current_ptr;
            }
        }

        ++l_img_comp;
        ++l_tccp;
        ++l_tile_comp;
    }

    return OPJ_TRUE;
}