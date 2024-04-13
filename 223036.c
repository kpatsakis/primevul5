OPJ_BOOL opj_j2k_encoder_set_extra_options(
    opj_j2k_t *p_j2k,
    const char* const* p_options,
    opj_event_mgr_t * p_manager)
{
    const char* const* p_option_iter;

    if (p_options == NULL) {
        return OPJ_TRUE;
    }

    for (p_option_iter = p_options; *p_option_iter != NULL; ++p_option_iter) {
        if (strncmp(*p_option_iter, "PLT=", 4) == 0) {
            if (strcmp(*p_option_iter, "PLT=YES") == 0) {
                p_j2k->m_specific_param.m_encoder.m_PLT = OPJ_TRUE;
            } else if (strcmp(*p_option_iter, "PLT=NO") == 0) {
                p_j2k->m_specific_param.m_encoder.m_PLT = OPJ_FALSE;
            } else {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Invalid value for option: %s.\n", *p_option_iter);
                return OPJ_FALSE;
            }
        } else {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Invalid option: %s.\n", *p_option_iter);
            return OPJ_FALSE;
        }
    }

    return OPJ_TRUE;
}