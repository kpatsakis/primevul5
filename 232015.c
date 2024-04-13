OPJ_SIZE_T opj_tcd_get_encoded_tile_size(opj_tcd_t *p_tcd)
{
    OPJ_UINT32 i;
    OPJ_SIZE_T l_data_size = 0;
    opj_image_comp_t * l_img_comp = 00;
    opj_tcd_tilecomp_t * l_tilec = 00;
    OPJ_UINT32 l_size_comp, l_remaining;

    l_tilec = p_tcd->tcd_image->tiles->comps;
    l_img_comp = p_tcd->image->comps;
    for (i = 0; i < p_tcd->image->numcomps; ++i) {
        l_size_comp = l_img_comp->prec >> 3; /*(/ 8)*/
        l_remaining = l_img_comp->prec & 7;  /* (%8) */

        if (l_remaining) {
            ++l_size_comp;
        }

        if (l_size_comp == 3) {
            l_size_comp = 4;
        }

        l_data_size += l_size_comp * ((OPJ_SIZE_T)(l_tilec->x1 - l_tilec->x0) *
                                      (OPJ_SIZE_T)(l_tilec->y1 - l_tilec->y0));
        ++l_img_comp;
        ++l_tilec;
    }

    return l_data_size;
}