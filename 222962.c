static OPJ_BOOL opj_j2k_calculate_tp(opj_j2k_t *p_j2k,
                                     opj_cp_t *cp,
                                     OPJ_UINT32 * p_nb_tiles,
                                     opj_image_t *image,
                                     opj_event_mgr_t * p_manager
                                    )
{
    OPJ_UINT32 pino, tileno;
    OPJ_UINT32 l_nb_tiles;
    opj_tcp_t *tcp;

    /* preconditions */
    assert(p_nb_tiles != 00);
    assert(cp != 00);
    assert(image != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    OPJ_UNUSED(p_j2k);
    OPJ_UNUSED(p_manager);

    l_nb_tiles = cp->tw * cp->th;
    * p_nb_tiles = 0;
    tcp = cp->tcps;

    /* INDEX >> */
    /* TODO mergeV2: check this part which use cstr_info */
    /*if (p_j2k->cstr_info) {
            opj_tile_info_t * l_info_tile_ptr = p_j2k->cstr_info->tile;

            for (tileno = 0; tileno < l_nb_tiles; ++tileno) {
                    OPJ_UINT32 cur_totnum_tp = 0;

                    opj_pi_update_encoding_parameters(image,cp,tileno);

                    for (pino = 0; pino <= tcp->numpocs; ++pino)
                    {
                            OPJ_UINT32 tp_num = opj_j2k_get_num_tp(cp,pino,tileno);

                            *p_nb_tiles = *p_nb_tiles + tp_num;

                            cur_totnum_tp += tp_num;
                    }

                    tcp->m_nb_tile_parts = cur_totnum_tp;

                    l_info_tile_ptr->tp = (opj_tp_info_t *) opj_malloc(cur_totnum_tp * sizeof(opj_tp_info_t));
                    if (l_info_tile_ptr->tp == 00) {
                            return OPJ_FALSE;
                    }

                    memset(l_info_tile_ptr->tp,0,cur_totnum_tp * sizeof(opj_tp_info_t));

                    l_info_tile_ptr->num_tps = cur_totnum_tp;

                    ++l_info_tile_ptr;
                    ++tcp;
            }
    }
    else */{
        for (tileno = 0; tileno < l_nb_tiles; ++tileno) {
            OPJ_UINT32 cur_totnum_tp = 0;

            opj_pi_update_encoding_parameters(image, cp, tileno);

            for (pino = 0; pino <= tcp->numpocs; ++pino) {
                OPJ_UINT32 tp_num = opj_j2k_get_num_tp(cp, pino, tileno);

                *p_nb_tiles = *p_nb_tiles + tp_num;

                cur_totnum_tp += tp_num;
            }
            tcp->m_nb_tile_parts = cur_totnum_tp;

            ++tcp;
        }
    }

    return OPJ_TRUE;
}