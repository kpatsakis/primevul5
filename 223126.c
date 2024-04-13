OPJ_BOOL opj_j2k_write_tile(opj_j2k_t * p_j2k,
                            OPJ_UINT32 p_tile_index,
                            OPJ_BYTE * p_data,
                            OPJ_UINT32 p_data_size,
                            opj_stream_private_t *p_stream,
                            opj_event_mgr_t * p_manager)
{
    if (! opj_j2k_pre_write_tile(p_j2k, p_tile_index, p_stream, p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Error while opj_j2k_pre_write_tile with tile index = %d\n", p_tile_index);
        return OPJ_FALSE;
    } else {
        OPJ_UINT32 j;
        /* Allocate data */
        for (j = 0; j < p_j2k->m_tcd->image->numcomps; ++j) {
            opj_tcd_tilecomp_t* l_tilec = p_j2k->m_tcd->tcd_image->tiles->comps + j;

            if (! opj_alloc_tile_component_data(l_tilec)) {
                opj_event_msg(p_manager, EVT_ERROR, "Error allocating tile component data.");
                return OPJ_FALSE;
            }
        }

        /* now copy data into the tile component */
        if (! opj_tcd_copy_tile_data(p_j2k->m_tcd, p_data, p_data_size)) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Size mismatch between tile data and sent data.");
            return OPJ_FALSE;
        }
        if (! opj_j2k_post_write_tile(p_j2k, p_stream, p_manager)) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "Error while opj_j2k_post_write_tile with tile index = %d\n", p_tile_index);
            return OPJ_FALSE;
        }
    }

    return OPJ_TRUE;
}