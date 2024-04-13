static OPJ_BOOL opj_j2k_merge_ppm(opj_cp_t *p_cp, opj_event_mgr_t * p_manager)
{
    OPJ_UINT32 i, l_ppm_data_size, l_N_ppm_remaining;

    /* preconditions */
    assert(p_cp != 00);
    assert(p_manager != 00);
    assert(p_cp->ppm_buffer == NULL);

    if (p_cp->ppm == 0U) {
        return OPJ_TRUE;
    }

    l_ppm_data_size = 0U;
    l_N_ppm_remaining = 0U;
    for (i = 0U; i < p_cp->ppm_markers_count; ++i) {
        if (p_cp->ppm_markers[i].m_data !=
                NULL) { /* standard doesn't seem to require contiguous Zppm */
            OPJ_UINT32 l_N_ppm;
            OPJ_UINT32 l_data_size = p_cp->ppm_markers[i].m_data_size;
            const OPJ_BYTE* l_data = p_cp->ppm_markers[i].m_data;

            if (l_N_ppm_remaining >= l_data_size) {
                l_N_ppm_remaining -= l_data_size;
                l_data_size = 0U;
            } else {
                l_data += l_N_ppm_remaining;
                l_data_size -= l_N_ppm_remaining;
                l_N_ppm_remaining = 0U;
            }

            if (l_data_size > 0U) {
                do {
                    /* read Nppm */
                    if (l_data_size < 4U) {
                        /* clean up to be done on l_cp destruction */
                        opj_event_msg(p_manager, EVT_ERROR, "Not enough bytes to read Nppm\n");
                        return OPJ_FALSE;
                    }
                    opj_read_bytes(l_data, &l_N_ppm, 4);
                    l_data += 4;
                    l_data_size -= 4;
                    l_ppm_data_size +=
                        l_N_ppm; /* can't overflow, max 256 markers of max 65536 bytes, that is when PPM markers are not corrupted which is checked elsewhere */

                    if (l_data_size >= l_N_ppm) {
                        l_data_size -= l_N_ppm;
                        l_data += l_N_ppm;
                    } else {
                        l_N_ppm_remaining = l_N_ppm - l_data_size;
                        l_data_size = 0U;
                    }
                } while (l_data_size > 0U);
            }
        }
    }

    if (l_N_ppm_remaining != 0U) {
        /* clean up to be done on l_cp destruction */
        opj_event_msg(p_manager, EVT_ERROR, "Corrupted PPM markers\n");
        return OPJ_FALSE;
    }

    p_cp->ppm_buffer = (OPJ_BYTE *) opj_malloc(l_ppm_data_size);
    if (p_cp->ppm_buffer == 00) {
        opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to read PPM marker\n");
        return OPJ_FALSE;
    }
    p_cp->ppm_len = l_ppm_data_size;
    l_ppm_data_size = 0U;
    l_N_ppm_remaining = 0U;
    for (i = 0U; i < p_cp->ppm_markers_count; ++i) {
        if (p_cp->ppm_markers[i].m_data !=
                NULL) { /* standard doesn't seem to require contiguous Zppm */
            OPJ_UINT32 l_N_ppm;
            OPJ_UINT32 l_data_size = p_cp->ppm_markers[i].m_data_size;
            const OPJ_BYTE* l_data = p_cp->ppm_markers[i].m_data;

            if (l_N_ppm_remaining >= l_data_size) {
                memcpy(p_cp->ppm_buffer + l_ppm_data_size, l_data, l_data_size);
                l_ppm_data_size += l_data_size;
                l_N_ppm_remaining -= l_data_size;
                l_data_size = 0U;
            } else {
                memcpy(p_cp->ppm_buffer + l_ppm_data_size, l_data, l_N_ppm_remaining);
                l_ppm_data_size += l_N_ppm_remaining;
                l_data += l_N_ppm_remaining;
                l_data_size -= l_N_ppm_remaining;
                l_N_ppm_remaining = 0U;
            }

            if (l_data_size > 0U) {
                do {
                    /* read Nppm */
                    if (l_data_size < 4U) {
                        /* clean up to be done on l_cp destruction */
                        opj_event_msg(p_manager, EVT_ERROR, "Not enough bytes to read Nppm\n");
                        return OPJ_FALSE;
                    }
                    opj_read_bytes(l_data, &l_N_ppm, 4);
                    l_data += 4;
                    l_data_size -= 4;

                    if (l_data_size >= l_N_ppm) {
                        memcpy(p_cp->ppm_buffer + l_ppm_data_size, l_data, l_N_ppm);
                        l_ppm_data_size += l_N_ppm;
                        l_data_size -= l_N_ppm;
                        l_data += l_N_ppm;
                    } else {
                        memcpy(p_cp->ppm_buffer + l_ppm_data_size, l_data, l_data_size);
                        l_ppm_data_size += l_data_size;
                        l_N_ppm_remaining = l_N_ppm - l_data_size;
                        l_data_size = 0U;
                    }
                } while (l_data_size > 0U);
            }
            opj_free(p_cp->ppm_markers[i].m_data);
            p_cp->ppm_markers[i].m_data = NULL;
            p_cp->ppm_markers[i].m_data_size = 0U;
        }
    }

    p_cp->ppm_data = p_cp->ppm_buffer;
    p_cp->ppm_data_size = p_cp->ppm_len;

    p_cp->ppm_markers_count = 0U;
    opj_free(p_cp->ppm_markers);
    p_cp->ppm_markers = NULL;

    return OPJ_TRUE;
}