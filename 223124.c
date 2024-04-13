static OPJ_UINT32 opj_j2k_get_max_coc_size(opj_j2k_t *p_j2k)
{
    OPJ_UINT32 i, j;
    OPJ_UINT32 l_nb_comp;
    OPJ_UINT32 l_nb_tiles;
    OPJ_UINT32 l_max = 0;

    /* preconditions */

    l_nb_tiles = p_j2k->m_cp.tw * p_j2k->m_cp.th ;
    l_nb_comp = p_j2k->m_private_image->numcomps;

    for (i = 0; i < l_nb_tiles; ++i) {
        for (j = 0; j < l_nb_comp; ++j) {
            l_max = opj_uint_max(l_max, opj_j2k_get_SPCod_SPCoc_size(p_j2k, i, j));
        }
    }

    return 6 + l_max;
}