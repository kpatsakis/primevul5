opj_j2k_t* opj_j2k_create_compress(void)
{
    opj_j2k_t *l_j2k = (opj_j2k_t*) opj_calloc(1, sizeof(opj_j2k_t));
    if (!l_j2k) {
        return NULL;
    }


    l_j2k->m_is_decoder = 0;
    l_j2k->m_cp.m_is_decoder = 0;

    l_j2k->m_specific_param.m_encoder.m_header_tile_data = (OPJ_BYTE *) opj_malloc(
                OPJ_J2K_DEFAULT_HEADER_SIZE);
    if (! l_j2k->m_specific_param.m_encoder.m_header_tile_data) {
        opj_j2k_destroy(l_j2k);
        return NULL;
    }

    l_j2k->m_specific_param.m_encoder.m_header_tile_data_size =
        OPJ_J2K_DEFAULT_HEADER_SIZE;

    /* validation list creation*/
    l_j2k->m_validation_list = opj_procedure_list_create();
    if (! l_j2k->m_validation_list) {
        opj_j2k_destroy(l_j2k);
        return NULL;
    }

    /* execution list creation*/
    l_j2k->m_procedure_list = opj_procedure_list_create();
    if (! l_j2k->m_procedure_list) {
        opj_j2k_destroy(l_j2k);
        return NULL;
    }

    l_j2k->m_tp = opj_thread_pool_create(opj_j2k_get_default_thread_count());
    if (!l_j2k->m_tp) {
        l_j2k->m_tp = opj_thread_pool_create(0);
    }
    if (!l_j2k->m_tp) {
        opj_j2k_destroy(l_j2k);
        return NULL;
    }

    return l_j2k;
}