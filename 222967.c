static OPJ_BOOL opj_j2k_read_qcc(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 l_num_comp, l_comp_no;

    /* preconditions */
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_num_comp = p_j2k->m_private_image->numcomps;

    if (l_num_comp <= 256) {
        if (p_header_size < 1) {
            opj_event_msg(p_manager, EVT_ERROR, "Error reading QCC marker\n");
            return OPJ_FALSE;
        }
        opj_read_bytes(p_header_data, &l_comp_no, 1);
        ++p_header_data;
        --p_header_size;
    } else {
        if (p_header_size < 2) {
            opj_event_msg(p_manager, EVT_ERROR, "Error reading QCC marker\n");
            return OPJ_FALSE;
        }
        opj_read_bytes(p_header_data, &l_comp_no, 2);
        p_header_data += 2;
        p_header_size -= 2;
    }

#ifdef USE_JPWL
    if (p_j2k->m_cp.correct) {

        static OPJ_UINT32 backup_compno = 0;

        /* compno is negative or larger than the number of components!!! */
        if (/*(l_comp_no < 0) ||*/ (l_comp_no >= l_num_comp)) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "JPWL: bad component number in QCC (%d out of a maximum of %d)\n",
                          l_comp_no, l_num_comp);
            if (!JPWL_ASSUME) {
                opj_event_msg(p_manager, EVT_ERROR, "JPWL: giving up\n");
                return OPJ_FALSE;
            }
            /* we try to correct */
            l_comp_no = backup_compno % l_num_comp;
            opj_event_msg(p_manager, EVT_WARNING, "- trying to adjust this\n"
                          "- setting component number to %d\n",
                          l_comp_no);
        }

        /* keep your private count of tiles */
        backup_compno++;
    };
#endif /* USE_JPWL */

    if (l_comp_no >= p_j2k->m_private_image->numcomps) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Invalid component number: %d, regarding the number of components %d\n",
                      l_comp_no, p_j2k->m_private_image->numcomps);
        return OPJ_FALSE;
    }

    if (! opj_j2k_read_SQcd_SQcc(p_j2k, l_comp_no, p_header_data, &p_header_size,
                                 p_manager)) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading QCC marker\n");
        return OPJ_FALSE;
    }

    if (p_header_size != 0) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading QCC marker\n");
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}