opj_codestream_index_t* j2k_get_cstr_index(opj_j2k_t* p_j2k)
{
    opj_codestream_index_t* l_cstr_index = (opj_codestream_index_t*)
                                           opj_calloc(1, sizeof(opj_codestream_index_t));
    if (!l_cstr_index) {
        return NULL;
    }

    l_cstr_index->main_head_start = p_j2k->cstr_index->main_head_start;
    l_cstr_index->main_head_end = p_j2k->cstr_index->main_head_end;
    l_cstr_index->codestream_size = p_j2k->cstr_index->codestream_size;

    l_cstr_index->marknum = p_j2k->cstr_index->marknum;
    l_cstr_index->marker = (opj_marker_info_t*)opj_malloc(l_cstr_index->marknum *
                           sizeof(opj_marker_info_t));
    if (!l_cstr_index->marker) {
        opj_free(l_cstr_index);
        return NULL;
    }

    if (p_j2k->cstr_index->marker) {
        memcpy(l_cstr_index->marker, p_j2k->cstr_index->marker,
               l_cstr_index->marknum * sizeof(opj_marker_info_t));
    } else {
        opj_free(l_cstr_index->marker);
        l_cstr_index->marker = NULL;
    }

    l_cstr_index->nb_of_tiles = p_j2k->cstr_index->nb_of_tiles;
    l_cstr_index->tile_index = (opj_tile_index_t*)opj_calloc(
                                   l_cstr_index->nb_of_tiles, sizeof(opj_tile_index_t));
    if (!l_cstr_index->tile_index) {
        opj_free(l_cstr_index->marker);
        opj_free(l_cstr_index);
        return NULL;
    }

    if (!p_j2k->cstr_index->tile_index) {
        opj_free(l_cstr_index->tile_index);
        l_cstr_index->tile_index = NULL;
    } else {
        OPJ_UINT32 it_tile = 0;
        for (it_tile = 0; it_tile < l_cstr_index->nb_of_tiles; it_tile++) {

            /* Tile Marker*/
            l_cstr_index->tile_index[it_tile].marknum =
                p_j2k->cstr_index->tile_index[it_tile].marknum;

            l_cstr_index->tile_index[it_tile].marker =
                (opj_marker_info_t*)opj_malloc(l_cstr_index->tile_index[it_tile].marknum *
                                               sizeof(opj_marker_info_t));

            if (!l_cstr_index->tile_index[it_tile].marker) {
                OPJ_UINT32 it_tile_free;

                for (it_tile_free = 0; it_tile_free < it_tile; it_tile_free++) {
                    opj_free(l_cstr_index->tile_index[it_tile_free].marker);
                }

                opj_free(l_cstr_index->tile_index);
                opj_free(l_cstr_index->marker);
                opj_free(l_cstr_index);
                return NULL;
            }

            if (p_j2k->cstr_index->tile_index[it_tile].marker)
                memcpy(l_cstr_index->tile_index[it_tile].marker,
                       p_j2k->cstr_index->tile_index[it_tile].marker,
                       l_cstr_index->tile_index[it_tile].marknum * sizeof(opj_marker_info_t));
            else {
                opj_free(l_cstr_index->tile_index[it_tile].marker);
                l_cstr_index->tile_index[it_tile].marker = NULL;
            }

            /* Tile part index*/
            l_cstr_index->tile_index[it_tile].nb_tps =
                p_j2k->cstr_index->tile_index[it_tile].nb_tps;

            l_cstr_index->tile_index[it_tile].tp_index =
                (opj_tp_index_t*)opj_malloc(l_cstr_index->tile_index[it_tile].nb_tps * sizeof(
                                                opj_tp_index_t));

            if (!l_cstr_index->tile_index[it_tile].tp_index) {
                OPJ_UINT32 it_tile_free;

                for (it_tile_free = 0; it_tile_free < it_tile; it_tile_free++) {
                    opj_free(l_cstr_index->tile_index[it_tile_free].marker);
                    opj_free(l_cstr_index->tile_index[it_tile_free].tp_index);
                }

                opj_free(l_cstr_index->tile_index);
                opj_free(l_cstr_index->marker);
                opj_free(l_cstr_index);
                return NULL;
            }

            if (p_j2k->cstr_index->tile_index[it_tile].tp_index) {
                memcpy(l_cstr_index->tile_index[it_tile].tp_index,
                       p_j2k->cstr_index->tile_index[it_tile].tp_index,
                       l_cstr_index->tile_index[it_tile].nb_tps * sizeof(opj_tp_index_t));
            } else {
                opj_free(l_cstr_index->tile_index[it_tile].tp_index);
                l_cstr_index->tile_index[it_tile].tp_index = NULL;
            }

            /* Packet index (NOT USED)*/
            l_cstr_index->tile_index[it_tile].nb_packet = 0;
            l_cstr_index->tile_index[it_tile].packet_index = NULL;

        }
    }

    return l_cstr_index;
}