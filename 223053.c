static OPJ_BOOL opj_j2k_read_cbd(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 l_nb_comp, l_num_comp;
    OPJ_UINT32 l_comp_def;
    OPJ_UINT32 i;
    opj_image_comp_t * l_comp = 00;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_num_comp = p_j2k->m_private_image->numcomps;

    if (p_header_size != (p_j2k->m_private_image->numcomps + 2)) {
        opj_event_msg(p_manager, EVT_ERROR, "Crror reading CBD marker\n");
        return OPJ_FALSE;
    }

    opj_read_bytes(p_header_data, &l_nb_comp,
                   2);                           /* Ncbd */
    p_header_data += 2;

    if (l_nb_comp != l_num_comp) {
        opj_event_msg(p_manager, EVT_ERROR, "Crror reading CBD marker\n");
        return OPJ_FALSE;
    }

    l_comp = p_j2k->m_private_image->comps;
    for (i = 0; i < l_num_comp; ++i) {
        opj_read_bytes(p_header_data, &l_comp_def,
                       1);                  /* Component bit depth */
        ++p_header_data;
        l_comp->sgnd = (l_comp_def >> 7) & 1;
        l_comp->prec = (l_comp_def & 0x7f) + 1;

        if (l_comp->prec > 31) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Invalid values for comp = %d : prec=%u (should be between 1 and 38 according to the JPEG2000 norm. OpenJpeg only supports up to 31)\n",
                          i, l_comp->prec);
            return OPJ_FALSE;
        }
        ++l_comp;
    }

    return OPJ_TRUE;
}