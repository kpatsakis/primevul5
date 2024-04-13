static OPJ_BOOL opj_j2k_are_all_used_components_decoded(opj_j2k_t *p_j2k,
        opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 compno;
    OPJ_BOOL decoded_all_used_components = OPJ_TRUE;

    if (p_j2k->m_specific_param.m_decoder.m_numcomps_to_decode) {
        for (compno = 0;
                compno < p_j2k->m_specific_param.m_decoder.m_numcomps_to_decode; compno++) {
            OPJ_UINT32 dec_compno =
                p_j2k->m_specific_param.m_decoder.m_comps_indices_to_decode[compno];
            if (p_j2k->m_output_image->comps[dec_compno].data == NULL) {
                opj_event_msg(p_manager, EVT_WARNING, "Failed to decode component %d\n",
                              dec_compno);
                decoded_all_used_components = OPJ_FALSE;
            }
        }
    } else {
        for (compno = 0; compno < p_j2k->m_output_image->numcomps; compno++) {
            if (p_j2k->m_output_image->comps[compno].data == NULL) {
                opj_event_msg(p_manager, EVT_WARNING, "Failed to decode component %d\n",
                              compno);
                decoded_all_used_components = OPJ_FALSE;
            }
        }
    }

    if (decoded_all_used_components == OPJ_FALSE) {
        opj_event_msg(p_manager, EVT_ERROR, "Failed to decode all used components\n");
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}