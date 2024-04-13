OPJ_BOOL opj_j2k_set_decoded_components(opj_j2k_t *p_j2k,
                                        OPJ_UINT32 numcomps,
                                        const OPJ_UINT32* comps_indices,
                                        opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 i;
    OPJ_BOOL* already_mapped;

    if (p_j2k->m_private_image == NULL) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "opj_read_header() should be called before "
                      "opj_set_decoded_components().\n");
        return OPJ_FALSE;
    }

    already_mapped = (OPJ_BOOL*) opj_calloc(sizeof(OPJ_BOOL),
                                            p_j2k->m_private_image->numcomps);
    if (already_mapped == NULL) {
        return OPJ_FALSE;
    }

    for (i = 0; i < numcomps; i++) {
        if (comps_indices[i] >= p_j2k->m_private_image->numcomps) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Invalid component index: %u\n",
                          comps_indices[i]);
            opj_free(already_mapped);
            return OPJ_FALSE;
        }
        if (already_mapped[comps_indices[i]]) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Component index %u used several times\n",
                          comps_indices[i]);
            opj_free(already_mapped);
            return OPJ_FALSE;
        }
        already_mapped[comps_indices[i]] = OPJ_TRUE;
    }
    opj_free(already_mapped);

    opj_free(p_j2k->m_specific_param.m_decoder.m_comps_indices_to_decode);
    if (numcomps) {
        p_j2k->m_specific_param.m_decoder.m_comps_indices_to_decode =
            (OPJ_UINT32*) opj_malloc(numcomps * sizeof(OPJ_UINT32));
        if (p_j2k->m_specific_param.m_decoder.m_comps_indices_to_decode == NULL) {
            p_j2k->m_specific_param.m_decoder.m_numcomps_to_decode = 0;
            return OPJ_FALSE;
        }
        memcpy(p_j2k->m_specific_param.m_decoder.m_comps_indices_to_decode,
               comps_indices,
               numcomps * sizeof(OPJ_UINT32));
    } else {
        p_j2k->m_specific_param.m_decoder.m_comps_indices_to_decode = NULL;
    }
    p_j2k->m_specific_param.m_decoder.m_numcomps_to_decode = numcomps;

    return OPJ_TRUE;
}