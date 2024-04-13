OPJ_UINT32 opj_tcd_get_decoded_tile_size(opj_tcd_t *p_tcd,
        OPJ_BOOL take_into_account_partial_decoding)
{
    OPJ_UINT32 i;
    OPJ_UINT32 l_data_size = 0;
    opj_image_comp_t * l_img_comp = 00;
    opj_tcd_tilecomp_t * l_tile_comp = 00;
    opj_tcd_resolution_t * l_res = 00;
    OPJ_UINT32 l_size_comp, l_remaining;
    OPJ_UINT32 l_temp;

    l_tile_comp = p_tcd->tcd_image->tiles->comps;
    l_img_comp = p_tcd->image->comps;

    for (i = 0; i < p_tcd->image->numcomps; ++i) {
        OPJ_UINT32 w, h;
        l_size_comp = l_img_comp->prec >> 3; /*(/ 8)*/
        l_remaining = l_img_comp->prec & 7;  /* (%8) */

        if (l_remaining) {
            ++l_size_comp;
        }

        if (l_size_comp == 3) {
            l_size_comp = 4;
        }

        l_res = l_tile_comp->resolutions + l_tile_comp->minimum_num_resolutions - 1;
        if (take_into_account_partial_decoding && !p_tcd->whole_tile_decoding) {
            w = l_res->win_x1 - l_res->win_x0;
            h = l_res->win_y1 - l_res->win_y0;
        } else {
            w = (OPJ_UINT32)(l_res->x1 - l_res->x0);
            h = (OPJ_UINT32)(l_res->y1 - l_res->y0);
        }
        if (h > 0 && UINT_MAX / w < h) {
            return UINT_MAX;
        }
        l_temp = w * h;
        if (l_size_comp && UINT_MAX / l_size_comp < l_temp) {
            return UINT_MAX;
        }
        l_temp *= l_size_comp;

        if (l_temp > UINT_MAX - l_data_size) {
            return UINT_MAX;
        }
        l_data_size += l_temp;
        ++l_img_comp;
        ++l_tile_comp;
    }

    return l_data_size;
}