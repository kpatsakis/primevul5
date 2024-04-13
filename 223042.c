static OPJ_UINT32 opj_j2k_get_max_poc_size(opj_j2k_t *p_j2k)
{
    opj_tcp_t * l_tcp = 00;
    OPJ_UINT32 l_nb_tiles = 0;
    OPJ_UINT32 l_max_poc = 0;
    OPJ_UINT32 i;

    l_tcp = p_j2k->m_cp.tcps;
    l_nb_tiles = p_j2k->m_cp.th * p_j2k->m_cp.tw;

    for (i = 0; i < l_nb_tiles; ++i) {
        l_max_poc = opj_uint_max(l_max_poc, l_tcp->numpocs);
        ++l_tcp;
    }

    ++l_max_poc;

    return 4 + 9 * l_max_poc;
}