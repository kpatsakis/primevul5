OPJ_BOOL opj_j2k_set_decoded_resolution_factor(opj_j2k_t *p_j2k,
        OPJ_UINT32 res_factor,
        opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 it_comp;

    p_j2k->m_cp.m_specific_param.m_dec.m_reduce = res_factor;

    if (p_j2k->m_private_image) {
        if (p_j2k->m_private_image->comps) {
            if (p_j2k->m_specific_param.m_decoder.m_default_tcp) {
                if (p_j2k->m_specific_param.m_decoder.m_default_tcp->tccps) {
                    for (it_comp = 0 ; it_comp < p_j2k->m_private_image->numcomps; it_comp++) {
                        OPJ_UINT32 max_res =
                            p_j2k->m_specific_param.m_decoder.m_default_tcp->tccps[it_comp].numresolutions;
                        if (res_factor >= max_res) {
                            opj_event_msg(p_manager, EVT_ERROR,
                                          "Resolution factor is greater than the maximum resolution in the component.\n");
                            return OPJ_FALSE;
                        }
                        p_j2k->m_private_image->comps[it_comp].factor = res_factor;
                    }
                    return OPJ_TRUE;
                }
            }
        }
    }

    return OPJ_FALSE;
}