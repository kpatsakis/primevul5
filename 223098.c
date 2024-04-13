static OPJ_BOOL opj_j2k_decode_tiles(opj_j2k_t *p_j2k,
                                     opj_stream_private_t *p_stream,
                                     opj_event_mgr_t * p_manager)
{
    OPJ_BOOL l_go_on = OPJ_TRUE;
    OPJ_UINT32 l_current_tile_no;
    OPJ_INT32 l_tile_x0, l_tile_y0, l_tile_x1, l_tile_y1;
    OPJ_UINT32 l_nb_comps;
    OPJ_UINT32 nr_tiles = 0;

    /* Particular case for whole single tile decoding */
    /* We can avoid allocating intermediate tile buffers */
    if (p_j2k->m_cp.tw == 1 && p_j2k->m_cp.th == 1 &&
            p_j2k->m_cp.tx0 == 0 && p_j2k->m_cp.ty0 == 0 &&
            p_j2k->m_output_image->x0 == 0 &&
            p_j2k->m_output_image->y0 == 0 &&
            p_j2k->m_output_image->x1 == p_j2k->m_cp.tdx &&
            p_j2k->m_output_image->y1 == p_j2k->m_cp.tdy) {
        OPJ_UINT32 i;
        if (! opj_j2k_read_tile_header(p_j2k,
                                       &l_current_tile_no,
                                       NULL,
                                       &l_tile_x0, &l_tile_y0,
                                       &l_tile_x1, &l_tile_y1,
                                       &l_nb_comps,
                                       &l_go_on,
                                       p_stream,
                                       p_manager)) {
            return OPJ_FALSE;
        }

        if (! opj_j2k_decode_tile(p_j2k, l_current_tile_no, NULL, 0,
                                  p_stream, p_manager)) {
            opj_event_msg(p_manager, EVT_ERROR, "Failed to decode tile 1/1\n");
            return OPJ_FALSE;
        }

        /* Transfer TCD data to output image data */
        for (i = 0; i < p_j2k->m_output_image->numcomps; i++) {
            opj_image_data_free(p_j2k->m_output_image->comps[i].data);
            p_j2k->m_output_image->comps[i].data =
                p_j2k->m_tcd->tcd_image->tiles->comps[i].data;
            p_j2k->m_output_image->comps[i].resno_decoded =
                p_j2k->m_tcd->image->comps[i].resno_decoded;
            p_j2k->m_tcd->tcd_image->tiles->comps[i].data = NULL;
        }

        return OPJ_TRUE;
    }

    for (;;) {
        if (p_j2k->m_cp.tw == 1 && p_j2k->m_cp.th == 1 &&
                p_j2k->m_cp.tcps[0].m_data != NULL) {
            l_current_tile_no = 0;
            p_j2k->m_current_tile_number = 0;
            p_j2k->m_specific_param.m_decoder.m_state |= J2K_STATE_DATA;
        } else {
            if (! opj_j2k_read_tile_header(p_j2k,
                                           &l_current_tile_no,
                                           NULL,
                                           &l_tile_x0, &l_tile_y0,
                                           &l_tile_x1, &l_tile_y1,
                                           &l_nb_comps,
                                           &l_go_on,
                                           p_stream,
                                           p_manager)) {
                return OPJ_FALSE;
            }

            if (! l_go_on) {
                break;
            }
        }

        if (! opj_j2k_decode_tile(p_j2k, l_current_tile_no, NULL, 0,
                                  p_stream, p_manager)) {
            opj_event_msg(p_manager, EVT_ERROR, "Failed to decode tile %d/%d\n",
                          l_current_tile_no + 1, p_j2k->m_cp.th * p_j2k->m_cp.tw);
            return OPJ_FALSE;
        }

        opj_event_msg(p_manager, EVT_INFO, "Tile %d/%d has been decoded.\n",
                      l_current_tile_no + 1, p_j2k->m_cp.th * p_j2k->m_cp.tw);

        if (! opj_j2k_update_image_data(p_j2k->m_tcd,
                                        p_j2k->m_output_image)) {
            return OPJ_FALSE;
        }

        if (p_j2k->m_cp.tw == 1 && p_j2k->m_cp.th == 1 &&
                !(p_j2k->m_output_image->x0 == p_j2k->m_private_image->x0 &&
                  p_j2k->m_output_image->y0 == p_j2k->m_private_image->y0 &&
                  p_j2k->m_output_image->x1 == p_j2k->m_private_image->x1 &&
                  p_j2k->m_output_image->y1 == p_j2k->m_private_image->y1)) {
            /* Keep current tcp data */
        } else {
            opj_j2k_tcp_data_destroy(&p_j2k->m_cp.tcps[l_current_tile_no]);
        }

        opj_event_msg(p_manager, EVT_INFO,
                      "Image data has been updated with tile %d.\n\n", l_current_tile_no + 1);

        if (opj_stream_get_number_byte_left(p_stream) == 0
                && p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_NEOC) {
            break;
        }
        if (++nr_tiles ==  p_j2k->m_cp.th * p_j2k->m_cp.tw) {
            break;
        }
    }

    if (! opj_j2k_are_all_used_components_decoded(p_j2k, p_manager)) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}