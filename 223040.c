static OPJ_BOOL opj_j2k_update_rates(opj_j2k_t *p_j2k,
                                     opj_stream_private_t *p_stream,
                                     opj_event_mgr_t * p_manager)
{
    opj_cp_t * l_cp = 00;
    opj_image_t * l_image = 00;
    opj_tcp_t * l_tcp = 00;
    opj_image_comp_t * l_img_comp = 00;

    OPJ_UINT32 i, j, k;
    OPJ_INT32 l_x0, l_y0, l_x1, l_y1;
    OPJ_FLOAT32 * l_rates = 0;
    OPJ_FLOAT32 l_sot_remove;
    OPJ_UINT32 l_bits_empty, l_size_pixel;
    OPJ_UINT64 l_tile_size = 0;
    OPJ_UINT32 l_last_res;
    OPJ_FLOAT32(* l_tp_stride_func)(opj_tcp_t *) = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    OPJ_UNUSED(p_manager);

    l_cp = &(p_j2k->m_cp);
    l_image = p_j2k->m_private_image;
    l_tcp = l_cp->tcps;

    l_bits_empty = 8 * l_image->comps->dx * l_image->comps->dy;
    l_size_pixel = l_image->numcomps * l_image->comps->prec;
    l_sot_remove = (OPJ_FLOAT32) opj_stream_tell(p_stream) / (OPJ_FLOAT32)(
                       l_cp->th * l_cp->tw);

    if (l_cp->m_specific_param.m_enc.m_tp_on) {
        l_tp_stride_func = opj_j2k_get_tp_stride;
    } else {
        l_tp_stride_func = opj_j2k_get_default_stride;
    }

    for (i = 0; i < l_cp->th; ++i) {
        for (j = 0; j < l_cp->tw; ++j) {
            OPJ_FLOAT32 l_offset = (OPJ_FLOAT32)(*l_tp_stride_func)(l_tcp) /
                                   (OPJ_FLOAT32)l_tcp->numlayers;

            /* 4 borders of the tile rescale on the image if necessary */
            l_x0 = opj_int_max((OPJ_INT32)(l_cp->tx0 + j * l_cp->tdx),
                               (OPJ_INT32)l_image->x0);
            l_y0 = opj_int_max((OPJ_INT32)(l_cp->ty0 + i * l_cp->tdy),
                               (OPJ_INT32)l_image->y0);
            l_x1 = opj_int_min((OPJ_INT32)(l_cp->tx0 + (j + 1) * l_cp->tdx),
                               (OPJ_INT32)l_image->x1);
            l_y1 = opj_int_min((OPJ_INT32)(l_cp->ty0 + (i + 1) * l_cp->tdy),
                               (OPJ_INT32)l_image->y1);

            l_rates = l_tcp->rates;

            /* Modification of the RATE >> */
            for (k = 0; k < l_tcp->numlayers; ++k) {
                if (*l_rates > 0.0f) {
                    *l_rates = (OPJ_FLOAT32)(((OPJ_FLOAT64)l_size_pixel * (OPJ_UINT32)(
                                                  l_x1 - l_x0) *
                                              (OPJ_UINT32)(l_y1 - l_y0))
                                             / ((*l_rates) * (OPJ_FLOAT32)l_bits_empty))
                               -
                               l_offset;
                }

                ++l_rates;
            }

            ++l_tcp;

        }
    }

    l_tcp = l_cp->tcps;

    for (i = 0; i < l_cp->th; ++i) {
        for (j = 0; j < l_cp->tw; ++j) {
            l_rates = l_tcp->rates;

            if (*l_rates > 0.0f) {
                *l_rates -= l_sot_remove;

                if (*l_rates < 30.0f) {
                    *l_rates = 30.0f;
                }
            }

            ++l_rates;

            l_last_res = l_tcp->numlayers - 1;

            for (k = 1; k < l_last_res; ++k) {

                if (*l_rates > 0.0f) {
                    *l_rates -= l_sot_remove;

                    if (*l_rates < * (l_rates - 1) + 10.0f) {
                        *l_rates  = (*(l_rates - 1)) + 20.0f;
                    }
                }

                ++l_rates;
            }

            if (*l_rates > 0.0f) {
                *l_rates -= (l_sot_remove + 2.f);

                if (*l_rates < * (l_rates - 1) + 10.0f) {
                    *l_rates  = (*(l_rates - 1)) + 20.0f;
                }
            }

            ++l_tcp;
        }
    }

    l_img_comp = l_image->comps;
    l_tile_size = 0;

    for (i = 0; i < l_image->numcomps; ++i) {
        l_tile_size += (OPJ_UINT64)opj_uint_ceildiv(l_cp->tdx, l_img_comp->dx)
                       *
                       opj_uint_ceildiv(l_cp->tdy, l_img_comp->dy)
                       *
                       l_img_comp->prec;

        ++l_img_comp;
    }

    /* TODO: where does this magic value come from ? */
    /* This used to be 1.3 / 8, but with random data and very small code */
    /* block sizes, this is not enough. For example with */
    /* bin/test_tile_encoder 1 256 256 32 32 8 0 reversible_with_precinct.j2k 4 4 3 0 0 1 16 16 */
    /* TODO revise this to take into account the overhead linked to the */
    /* number of packets and number of code blocks in packets */
    l_tile_size = (OPJ_UINT64)((double)l_tile_size * 1.4 / 8);

    /* Arbitrary amount to make the following work: */
    /* bin/test_tile_encoder 1 256 256 17 16 8 0 reversible_no_precinct.j2k 4 4 3 0 0 1 */
    l_tile_size += 500;

    l_tile_size += opj_j2k_get_specific_header_sizes(p_j2k);

    if (l_tile_size > UINT_MAX) {
        l_tile_size = UINT_MAX;
    }

    p_j2k->m_specific_param.m_encoder.m_encoded_tile_size = (OPJ_UINT32)l_tile_size;
    p_j2k->m_specific_param.m_encoder.m_encoded_tile_data =
        (OPJ_BYTE *) opj_malloc(p_j2k->m_specific_param.m_encoder.m_encoded_tile_size);
    if (p_j2k->m_specific_param.m_encoder.m_encoded_tile_data == 00) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Not enough memory to allocate m_encoded_tile_data. %u MB required\n",
                      (OPJ_UINT32)(l_tile_size / 1024 / 1024));
        return OPJ_FALSE;
    }

    if (OPJ_IS_CINEMA(l_cp->rsiz) || OPJ_IS_IMF(l_cp->rsiz)) {
        p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_buffer =
            (OPJ_BYTE *) opj_malloc(5 *
                                    p_j2k->m_specific_param.m_encoder.m_total_tile_parts);
        if (! p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_buffer) {
            return OPJ_FALSE;
        }

        p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_current =
            p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_buffer;
    }

    return OPJ_TRUE;
}