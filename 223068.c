static OPJ_BOOL opj_j2k_read_ppt(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    opj_cp_t *l_cp = 00;
    opj_tcp_t *l_tcp = 00;
    OPJ_UINT32 l_Z_ppt;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    /* We need to have the Z_ppt element + 1 byte of Ippt at minimum */
    if (p_header_size < 2) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading PPT marker\n");
        return OPJ_FALSE;
    }

    l_cp = &(p_j2k->m_cp);
    if (l_cp->ppm) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Error reading PPT marker: packet header have been previously found in the main header (PPM marker).\n");
        return OPJ_FALSE;
    }

    l_tcp = &(l_cp->tcps[p_j2k->m_current_tile_number]);
    l_tcp->ppt = 1;

    opj_read_bytes(p_header_data, &l_Z_ppt, 1);             /* Z_ppt */
    ++p_header_data;
    --p_header_size;

    /* check allocation needed */
    if (l_tcp->ppt_markers == NULL) { /* first PPT marker */
        OPJ_UINT32 l_newCount = l_Z_ppt + 1U; /* can't overflow, l_Z_ppt is UINT8 */
        assert(l_tcp->ppt_markers_count == 0U);

        l_tcp->ppt_markers = (opj_ppx *) opj_calloc(l_newCount, sizeof(opj_ppx));
        if (l_tcp->ppt_markers == NULL) {
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read PPT marker\n");
            return OPJ_FALSE;
        }
        l_tcp->ppt_markers_count = l_newCount;
    } else if (l_tcp->ppt_markers_count <= l_Z_ppt) {
        OPJ_UINT32 l_newCount = l_Z_ppt + 1U; /* can't overflow, l_Z_ppt is UINT8 */
        opj_ppx *new_ppt_markers;
        new_ppt_markers = (opj_ppx *) opj_realloc(l_tcp->ppt_markers,
                          l_newCount * sizeof(opj_ppx));
        if (new_ppt_markers == NULL) {
            /* clean up to be done on l_tcp destruction */
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read PPT marker\n");
            return OPJ_FALSE;
        }
        l_tcp->ppt_markers = new_ppt_markers;
        memset(l_tcp->ppt_markers + l_tcp->ppt_markers_count, 0,
               (l_newCount - l_tcp->ppt_markers_count) * sizeof(opj_ppx));
        l_tcp->ppt_markers_count = l_newCount;
    }

    if (l_tcp->ppt_markers[l_Z_ppt].m_data != NULL) {
        /* clean up to be done on l_tcp destruction */
        opj_event_msg(p_manager, EVT_ERROR, "Zppt %u already read\n", l_Z_ppt);
        return OPJ_FALSE;
    }

    l_tcp->ppt_markers[l_Z_ppt].m_data = (OPJ_BYTE *) opj_malloc(p_header_size);
    if (l_tcp->ppt_markers[l_Z_ppt].m_data == NULL) {
        /* clean up to be done on l_tcp destruction */
        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read PPT marker\n");
        return OPJ_FALSE;
    }
    l_tcp->ppt_markers[l_Z_ppt].m_data_size = p_header_size;
    memcpy(l_tcp->ppt_markers[l_Z_ppt].m_data, p_header_data, p_header_size);
    return OPJ_TRUE;
}