static OPJ_BOOL opj_j2k_read_rgn(opj_j2k_t *p_j2k,
                                 OPJ_BYTE * p_header_data,
                                 OPJ_UINT32 p_header_size,
                                 opj_event_mgr_t * p_manager
                                )
{
    OPJ_UINT32 l_nb_comp;
    opj_image_t * l_image = 00;

    opj_cp_t *l_cp = 00;
    opj_tcp_t *l_tcp = 00;
    OPJ_UINT32 l_comp_room, l_comp_no, l_roi_sty;

    /* preconditions*/
    assert(p_header_data != 00);
    assert(p_j2k != 00);
    assert(p_manager != 00);

    l_image = p_j2k->m_private_image;
    l_nb_comp = l_image->numcomps;

    if (l_nb_comp <= 256) {
        l_comp_room = 1;
    } else {
        l_comp_room = 2;
    }

    if (p_header_size != 2 + l_comp_room) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading RGN marker\n");
        return OPJ_FALSE;
    }

    l_cp = &(p_j2k->m_cp);
    l_tcp = (p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH) ?
            &l_cp->tcps[p_j2k->m_current_tile_number] :
            p_j2k->m_specific_param.m_decoder.m_default_tcp;

    opj_read_bytes(p_header_data, &l_comp_no, l_comp_room);         /* Crgn */
    p_header_data += l_comp_room;
    opj_read_bytes(p_header_data, &l_roi_sty,
                   1);                                     /* Srgn */
    ++p_header_data;

#ifdef USE_JPWL
    if (l_cp->correct) {
        /* totlen is negative or larger than the bytes left!!! */
        if (l_comp_room >= l_nb_comp) {
            opj_event_msg(p_manager, EVT_ERROR,
                          "JPWL: bad component number in RGN (%d when there are only %d)\n",
                          l_comp_room, l_nb_comp);
            if (!JPWL_ASSUME) {
                opj_event_msg(p_manager, EVT_ERROR, "JPWL: giving up\n");
                return OPJ_FALSE;
            }
        }
    };
#endif /* USE_JPWL */

    /* testcase 3635.pdf.asan.77.2930 */
    if (l_comp_no >= l_nb_comp) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "bad component number in RGN (%d when there are only %d)\n",
                      l_comp_no, l_nb_comp);
        return OPJ_FALSE;
    }

    opj_read_bytes(p_header_data,
                   (OPJ_UINT32 *)(&(l_tcp->tccps[l_comp_no].roishift)), 1);  /* SPrgn */
    ++p_header_data;

    return OPJ_TRUE;

}