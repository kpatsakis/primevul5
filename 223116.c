static OPJ_UINT32 opj_j2k_get_max_toc_size(opj_j2k_t *p_j2k)
{
    OPJ_UINT32 i;
    OPJ_UINT32 l_nb_tiles;
    OPJ_UINT32 l_max = 0;
    opj_tcp_t * l_tcp = 00;

    l_tcp = p_j2k->m_cp.tcps;
    l_nb_tiles = p_j2k->m_cp.tw * p_j2k->m_cp.th ;

    for (i = 0; i < l_nb_tiles; ++i) {
        l_max = opj_uint_max(l_max, l_tcp->m_nb_tile_parts);

        ++l_tcp;
    }

    return 12 * l_max;
}