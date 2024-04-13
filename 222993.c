static void opj_get_tile_dimensions(opj_image_t * l_image,
                                    opj_tcd_tilecomp_t * l_tilec,
                                    opj_image_comp_t * l_img_comp,
                                    OPJ_UINT32* l_size_comp,
                                    OPJ_UINT32* l_width,
                                    OPJ_UINT32* l_height,
                                    OPJ_UINT32* l_offset_x,
                                    OPJ_UINT32* l_offset_y,
                                    OPJ_UINT32* l_image_width,
                                    OPJ_UINT32* l_stride,
                                    OPJ_UINT32* l_tile_offset)
{
    OPJ_UINT32 l_remaining;
    *l_size_comp = l_img_comp->prec >> 3; /* (/8) */
    l_remaining = l_img_comp->prec & 7;  /* (%8) */
    if (l_remaining) {
        *l_size_comp += 1;
    }

    if (*l_size_comp == 3) {
        *l_size_comp = 4;
    }

    *l_width  = (OPJ_UINT32)(l_tilec->x1 - l_tilec->x0);
    *l_height = (OPJ_UINT32)(l_tilec->y1 - l_tilec->y0);
    *l_offset_x = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)l_image->x0,
                  (OPJ_INT32)l_img_comp->dx);
    *l_offset_y = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)l_image->y0,
                  (OPJ_INT32)l_img_comp->dy);
    *l_image_width = (OPJ_UINT32)opj_int_ceildiv((OPJ_INT32)l_image->x1 -
                     (OPJ_INT32)l_image->x0, (OPJ_INT32)l_img_comp->dx);
    *l_stride = *l_image_width - *l_width;
    *l_tile_offset = ((OPJ_UINT32)l_tilec->x0 - *l_offset_x) + ((
                         OPJ_UINT32)l_tilec->y0 - *l_offset_y) * *l_image_width;
}