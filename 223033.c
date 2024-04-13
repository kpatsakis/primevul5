void opj_j2k_setup_decoder(opj_j2k_t *j2k, opj_dparameters_t *parameters)
{
    if (j2k && parameters) {
        j2k->m_cp.m_specific_param.m_dec.m_layer = parameters->cp_layer;
        j2k->m_cp.m_specific_param.m_dec.m_reduce = parameters->cp_reduce;

        j2k->dump_state = (parameters->flags & OPJ_DPARAMETERS_DUMP_FLAG);
#ifdef USE_JPWL
        j2k->m_cp.correct = parameters->jpwl_correct;
        j2k->m_cp.exp_comps = parameters->jpwl_exp_comps;
        j2k->m_cp.max_tiles = parameters->jpwl_max_tiles;
#endif /* USE_JPWL */
    }
}