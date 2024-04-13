static OPJ_BOOL opj_tcd_mct_decode(opj_tcd_t *p_tcd, opj_event_mgr_t *p_manager)
{
    opj_tcd_tile_t * l_tile = p_tcd->tcd_image->tiles;
    opj_tcp_t * l_tcp = p_tcd->tcp;
    opj_tcd_tilecomp_t * l_tile_comp = l_tile->comps;
    OPJ_SIZE_T l_samples;
    OPJ_UINT32 i;

    if (l_tcp->mct == 0 || p_tcd->used_component != NULL) {
        return OPJ_TRUE;
    }

    if (p_tcd->whole_tile_decoding) {
        opj_tcd_resolution_t* res_comp0 = l_tile->comps[0].resolutions +
                                          l_tile_comp->minimum_num_resolutions - 1;

        /* A bit inefficient: we process more data than needed if */
        /* resno_decoded < l_tile_comp->minimum_num_resolutions-1, */
        /* but we would need to take into account a stride then */
        l_samples = (OPJ_SIZE_T)(res_comp0->x1 - res_comp0->x0) *
                    (OPJ_SIZE_T)(res_comp0->y1 - res_comp0->y0);
        if (l_tile->numcomps >= 3) {
            if (l_tile_comp->minimum_num_resolutions !=
                    l_tile->comps[1].minimum_num_resolutions ||
                    l_tile_comp->minimum_num_resolutions !=
                    l_tile->comps[2].minimum_num_resolutions) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Tiles don't all have the same dimension. Skip the MCT step.\n");
                return OPJ_FALSE;
            }
        }
        if (l_tile->numcomps >= 3) {
            opj_tcd_resolution_t* res_comp1 = l_tile->comps[1].resolutions +
                                              l_tile_comp->minimum_num_resolutions - 1;
            opj_tcd_resolution_t* res_comp2 = l_tile->comps[2].resolutions +
                                              l_tile_comp->minimum_num_resolutions - 1;
            /* testcase 1336.pdf.asan.47.376 */
            if (p_tcd->image->comps[0].resno_decoded !=
                    p_tcd->image->comps[1].resno_decoded ||
                    p_tcd->image->comps[0].resno_decoded !=
                    p_tcd->image->comps[2].resno_decoded ||
                    (OPJ_SIZE_T)(res_comp1->x1 - res_comp1->x0) *
                    (OPJ_SIZE_T)(res_comp1->y1 - res_comp1->y0) != l_samples ||
                    (OPJ_SIZE_T)(res_comp2->x1 - res_comp2->x0) *
                    (OPJ_SIZE_T)(res_comp2->y1 - res_comp2->y0) != l_samples) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Tiles don't all have the same dimension. Skip the MCT step.\n");
                return OPJ_FALSE;
            }
        }
    } else {
        opj_tcd_resolution_t* res_comp0 = l_tile->comps[0].resolutions +
                                          p_tcd->image->comps[0].resno_decoded;

        l_samples = (OPJ_SIZE_T)(res_comp0->win_x1 - res_comp0->win_x0) *
                    (OPJ_SIZE_T)(res_comp0->win_y1 - res_comp0->win_y0);
        if (l_tile->numcomps >= 3) {
            opj_tcd_resolution_t* res_comp1 = l_tile->comps[1].resolutions +
                                              p_tcd->image->comps[1].resno_decoded;
            opj_tcd_resolution_t* res_comp2 = l_tile->comps[2].resolutions +
                                              p_tcd->image->comps[2].resno_decoded;
            /* testcase 1336.pdf.asan.47.376 */
            if (p_tcd->image->comps[0].resno_decoded !=
                    p_tcd->image->comps[1].resno_decoded ||
                    p_tcd->image->comps[0].resno_decoded !=
                    p_tcd->image->comps[2].resno_decoded ||
                    (OPJ_SIZE_T)(res_comp1->win_x1 - res_comp1->win_x0) *
                    (OPJ_SIZE_T)(res_comp1->win_y1 - res_comp1->win_y0) != l_samples ||
                    (OPJ_SIZE_T)(res_comp2->win_x1 - res_comp2->win_x0) *
                    (OPJ_SIZE_T)(res_comp2->win_y1 - res_comp2->win_y0) != l_samples) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Tiles don't all have the same dimension. Skip the MCT step.\n");
                return OPJ_FALSE;
            }
        }
    }

    if (l_tile->numcomps >= 3) {
        if (l_tcp->mct == 2) {
            OPJ_BYTE ** l_data;

            if (! l_tcp->m_mct_decoding_matrix) {
                return OPJ_TRUE;
            }

            l_data = (OPJ_BYTE **) opj_malloc(l_tile->numcomps * sizeof(OPJ_BYTE*));
            if (! l_data) {
                return OPJ_FALSE;
            }

            for (i = 0; i < l_tile->numcomps; ++i) {
                if (p_tcd->whole_tile_decoding) {
                    l_data[i] = (OPJ_BYTE*) l_tile_comp->data;
                } else {
                    l_data[i] = (OPJ_BYTE*) l_tile_comp->data_win;
                }
                ++l_tile_comp;
            }

            if (! opj_mct_decode_custom(/* MCT data */
                        (OPJ_BYTE*) l_tcp->m_mct_decoding_matrix,
                        /* size of components */
                        l_samples,
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
        } else {
            if (l_tcp->tccps->qmfbid == 1) {
                if (p_tcd->whole_tile_decoding) {
                    opj_mct_decode(l_tile->comps[0].data,
                                   l_tile->comps[1].data,
                                   l_tile->comps[2].data,
                                   l_samples);
                } else {
                    opj_mct_decode(l_tile->comps[0].data_win,
                                   l_tile->comps[1].data_win,
                                   l_tile->comps[2].data_win,
                                   l_samples);
                }
            } else {
                if (p_tcd->whole_tile_decoding) {
                    opj_mct_decode_real((OPJ_FLOAT32*)l_tile->comps[0].data,
                                        (OPJ_FLOAT32*)l_tile->comps[1].data,
                                        (OPJ_FLOAT32*)l_tile->comps[2].data,
                                        l_samples);
                } else {
                    opj_mct_decode_real((OPJ_FLOAT32*)l_tile->comps[0].data_win,
                                        (OPJ_FLOAT32*)l_tile->comps[1].data_win,
                                        (OPJ_FLOAT32*)l_tile->comps[2].data_win,
                                        l_samples);
                }
            }
        }
    } else {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Number of components (%d) is inconsistent with a MCT. Skip the MCT step.\n",
                      l_tile->numcomps);
    }

    return OPJ_TRUE;
}