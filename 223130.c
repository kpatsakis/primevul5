static OPJ_BOOL opj_j2k_read_ppm(
    opj_j2k_t *p_j2k,
    OPJ_BYTE * p_header_data,
    OPJ_UINT32 p_header_size,
    opj_event_mgr_t * p_manager)
{
    opj_cp_t *l_cp = 00;
    OPJ_UINT32 l_Z_ppm;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    /* We need to have the Z_ppm element + 1 byte of Nppm/Ippm at minimum */
    if (p_header_size < 2) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading PPM marker\n");
        return OPJ_FALSE;
    }

    l_cp = &(p_j2k->m_cp);
    l_cp->ppm = 1;

    opj_read_bytes(p_header_data, &l_Z_ppm, 1);             /* Z_ppm */
    ++p_header_data;
    --p_header_size;

    /* check allocation needed */
    if (l_cp->ppm_markers == NULL) { /* first PPM marker */
        OPJ_UINT32 l_newCount = l_Z_ppm + 1U; /* can't overflow, l_Z_ppm is UINT8 */
        assert(l_cp->ppm_markers_count == 0U);

        l_cp->ppm_markers = (opj_ppx *) opj_calloc(l_newCount, sizeof(opj_ppx));
        if (l_cp->ppm_markers == NULL) {
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read PPM marker\n");
            return OPJ_FALSE;
        }
        l_cp->ppm_markers_count = l_newCount;
    } else if (l_cp->ppm_markers_count <= l_Z_ppm) {
        OPJ_UINT32 l_newCount = l_Z_ppm + 1U; /* can't overflow, l_Z_ppm is UINT8 */
        opj_ppx *new_ppm_markers;
        new_ppm_markers = (opj_ppx *) opj_realloc(l_cp->ppm_markers,
                          l_newCount * sizeof(opj_ppx));
        if (new_ppm_markers == NULL) {
            /* clean up to be done on l_cp destruction */
            opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read PPM marker\n");
            return OPJ_FALSE;
        }
        l_cp->ppm_markers = new_ppm_markers;
        memset(l_cp->ppm_markers + l_cp->ppm_markers_count, 0,
               (l_newCount - l_cp->ppm_markers_count) * sizeof(opj_ppx));
        l_cp->ppm_markers_count = l_newCount;
    }

    if (l_cp->ppm_markers[l_Z_ppm].m_data != NULL) {
        /* clean up to be done on l_cp destruction */
        opj_event_msg(p_manager, EVT_ERROR, "Zppm %u already read\n", l_Z_ppm);
        return OPJ_FALSE;
    }

    l_cp->ppm_markers[l_Z_ppm].m_data = (OPJ_BYTE *) opj_malloc(p_header_size);
    if (l_cp->ppm_markers[l_Z_ppm].m_data == NULL) {
        /* clean up to be done on l_cp destruction */
        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read PPM marker\n");
        return OPJ_FALSE;
    }
    l_cp->ppm_markers[l_Z_ppm].m_data_size = p_header_size;
    memcpy(l_cp->ppm_markers[l_Z_ppm].m_data, p_header_data, p_header_size);

    return OPJ_TRUE;
}