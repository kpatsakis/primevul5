static OPJ_BOOL opj_j2k_allocate_tile_element_cstr_index(opj_j2k_t *p_j2k)
{
    OPJ_UINT32 it_tile = 0;

    p_j2k->cstr_index->nb_of_tiles = p_j2k->m_cp.tw * p_j2k->m_cp.th;
    p_j2k->cstr_index->tile_index = (opj_tile_index_t*)opj_calloc(
                                        p_j2k->cstr_index->nb_of_tiles, sizeof(opj_tile_index_t));
    if (!p_j2k->cstr_index->tile_index) {
        return OPJ_FALSE;
    }

    for (it_tile = 0; it_tile < p_j2k->cstr_index->nb_of_tiles; it_tile++) {
        p_j2k->cstr_index->tile_index[it_tile].maxmarknum = 100;
        p_j2k->cstr_index->tile_index[it_tile].marknum = 0;
        p_j2k->cstr_index->tile_index[it_tile].marker = (opj_marker_info_t*)
                opj_calloc(p_j2k->cstr_index->tile_index[it_tile].maxmarknum,
                           sizeof(opj_marker_info_t));
        if (!p_j2k->cstr_index->tile_index[it_tile].marker) {
            return OPJ_FALSE;
        }
    }

    return OPJ_TRUE;
}