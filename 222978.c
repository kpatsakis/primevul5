static OPJ_BOOL opj_j2k_write_cbd(opj_j2k_t *p_j2k,
                                  struct opj_stream_private *p_stream,
                                  struct opj_event_mgr * p_manager)
{
    OPJ_UINT32 i;
    OPJ_UINT32 l_cbd_size;
    OPJ_BYTE * l_current_data = 00;
    opj_image_t *l_image = 00;
    opj_image_comp_t * l_comp = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    l_image = p_j2k->m_private_image;
    l_cbd_size = 6 + p_j2k->m_private_image->numcomps;

    if (l_cbd_size > p_j2k->m_specific_param.m_encoder.m_header_tile_data_size) {
        OPJ_BYTE *new_header_tile_data = (OPJ_BYTE *) opj_realloc(
                                             p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_cbd_size);
        if (! new_header_tile_data) {
            opj_free(p_j2k->m_specific_param.m_encoder.m_header_tile_data);
            p_j2k->m_specific_param.m_encoder.m_header_tile_data = NULL;
            p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = 0;
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to write CBD marker\n");
            return OPJ_FALSE;
        }
        p_j2k->m_specific_param.m_encoder.m_header_tile_data = new_header_tile_data;
        p_j2k->m_specific_param.m_encoder.m_header_tile_data_size = l_cbd_size;
    }

    l_current_data = p_j2k->m_specific_param.m_encoder.m_header_tile_data;

    opj_write_bytes(l_current_data, J2K_MS_CBD, 2);                 /* CBD */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_cbd_size - 2, 2);             /* L_CBD */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_image->numcomps, 2);          /* Ncbd */
    l_current_data += 2;

    l_comp = l_image->comps;

    for (i = 0; i < l_image->numcomps; ++i) {
        opj_write_bytes(l_current_data, (l_comp->sgnd << 7) | (l_comp->prec - 1),
                        1);           /* Component bit depth */
        ++l_current_data;

        ++l_comp;
    }

    if (opj_stream_write_data(p_stream,
                              p_j2k->m_specific_param.m_encoder.m_header_tile_data, l_cbd_size,
                              p_manager) != l_cbd_size) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}