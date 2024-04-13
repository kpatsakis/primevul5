static void opj_j2k_cp_destroy(opj_cp_t *p_cp)
{
    OPJ_UINT32 l_nb_tiles;
    opj_tcp_t * l_current_tile = 00;

    if (p_cp == 00) {
        return;
    }
    if (p_cp->tcps != 00) {
        OPJ_UINT32 i;
        l_current_tile = p_cp->tcps;
        l_nb_tiles = p_cp->th * p_cp->tw;

        for (i = 0U; i < l_nb_tiles; ++i) {
            opj_j2k_tcp_destroy(l_current_tile);
            ++l_current_tile;
        }
        opj_free(p_cp->tcps);
        p_cp->tcps = 00;
    }
    if (p_cp->ppm_markers != 00) {
        OPJ_UINT32 i;
        for (i = 0U; i < p_cp->ppm_markers_count; ++i) {
            if (p_cp->ppm_markers[i].m_data != NULL) {
                opj_free(p_cp->ppm_markers[i].m_data);
            }
        }
        p_cp->ppm_markers_count = 0U;
        opj_free(p_cp->ppm_markers);
        p_cp->ppm_markers = NULL;
    }
    opj_free(p_cp->ppm_buffer);
    p_cp->ppm_buffer = 00;
    p_cp->ppm_data =
        NULL; /* ppm_data belongs to the allocated buffer pointed by ppm_buffer */
    opj_free(p_cp->comment);
    p_cp->comment = 00;
    if (! p_cp->m_is_decoder) {
        opj_free(p_cp->m_specific_param.m_enc.m_matrice);
        p_cp->m_specific_param.m_enc.m_matrice = 00;
    }
}