static OPJ_BOOL opj_tcd_rate_allocate_encode(opj_tcd_t *p_tcd,
        OPJ_BYTE * p_dest_data,
        OPJ_UINT32 p_max_dest_size,
        opj_codestream_info_t *p_cstr_info,
        opj_event_mgr_t *p_manager)
{
    opj_cp_t * l_cp = p_tcd->cp;
    OPJ_UINT32 l_nb_written = 0;

    if (p_cstr_info)  {
        p_cstr_info->index_write = 0;
    }

    if (l_cp->m_specific_param.m_enc.m_disto_alloc ||
            l_cp->m_specific_param.m_enc.m_fixed_quality)  {
        /* fixed_quality */
        /* Normal Rate/distortion allocation */
        if (! opj_tcd_rateallocate(p_tcd, p_dest_data, &l_nb_written, p_max_dest_size,
                                   p_cstr_info, p_manager)) {
            return OPJ_FALSE;
        }
    } else {
        /* Fixed layer allocation */
        opj_tcd_rateallocate_fixed(p_tcd);
    }

    return OPJ_TRUE;
}