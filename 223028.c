static void opj_j2k_get_tile_data(opj_tcd_t * p_tcd, OPJ_BYTE * p_data)
{
    OPJ_UINT32 i, j, k = 0;

    for (i = 0; i < p_tcd->image->numcomps; ++i) {
        opj_image_t * l_image =  p_tcd->image;
        OPJ_INT32 * l_src_ptr;
        opj_tcd_tilecomp_t * l_tilec = p_tcd->tcd_image->tiles->comps + i;
        opj_image_comp_t * l_img_comp = l_image->comps + i;
        OPJ_UINT32 l_size_comp, l_width, l_height, l_offset_x, l_offset_y,
                   l_image_width, l_stride, l_tile_offset;

        opj_get_tile_dimensions(l_image,
                                l_tilec,
                                l_img_comp,
                                &l_size_comp,
                                &l_width,
                                &l_height,
                                &l_offset_x,
                                &l_offset_y,
                                &l_image_width,
                                &l_stride,
                                &l_tile_offset);

        l_src_ptr = l_img_comp->data + l_tile_offset;

        switch (l_size_comp) {
        case 1: {
            OPJ_CHAR * l_dest_ptr = (OPJ_CHAR*) p_data;
            if (l_img_comp->sgnd) {
                for (j = 0; j < l_height; ++j) {
                    for (k = 0; k < l_width; ++k) {
                        *(l_dest_ptr) = (OPJ_CHAR)(*l_src_ptr);
                        ++l_dest_ptr;
                        ++l_src_ptr;
                    }
                    l_src_ptr += l_stride;
                }
            } else {
                for (j = 0; j < l_height; ++j) {
                    for (k = 0; k < l_width; ++k) {
                        *(l_dest_ptr) = (OPJ_CHAR)((*l_src_ptr) & 0xff);
                        ++l_dest_ptr;
                        ++l_src_ptr;
                    }
                    l_src_ptr += l_stride;
                }
            }

            p_data = (OPJ_BYTE*) l_dest_ptr;
        }
        break;
        case 2: {
            OPJ_INT16 * l_dest_ptr = (OPJ_INT16 *) p_data;
            if (l_img_comp->sgnd) {
                for (j = 0; j < l_height; ++j) {
                    for (k = 0; k < l_width; ++k) {
                        *(l_dest_ptr++) = (OPJ_INT16)(*(l_src_ptr++));
                    }
                    l_src_ptr += l_stride;
                }
            } else {
                for (j = 0; j < l_height; ++j) {
                    for (k = 0; k < l_width; ++k) {
                        *(l_dest_ptr++) = (OPJ_INT16)((*(l_src_ptr++)) & 0xffff);
                    }
                    l_src_ptr += l_stride;
                }
            }

            p_data = (OPJ_BYTE*) l_dest_ptr;
        }
        break;
        case 4: {
            OPJ_INT32 * l_dest_ptr = (OPJ_INT32 *) p_data;
            for (j = 0; j < l_height; ++j) {
                for (k = 0; k < l_width; ++k) {
                    *(l_dest_ptr++) = *(l_src_ptr++);
                }
                l_src_ptr += l_stride;
            }

            p_data = (OPJ_BYTE*) l_dest_ptr;
        }
        break;
        }
    }
}