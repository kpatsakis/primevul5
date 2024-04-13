static OPJ_BOOL opj_j2k_get_sot_values(OPJ_BYTE *  p_header_data,
                                       OPJ_UINT32  p_header_size,
                                       OPJ_UINT32* p_tile_no,
                                       OPJ_UINT32* p_tot_len,
                                       OPJ_UINT32* p_current_part,
                                       OPJ_UINT32* p_num_parts,
                                       opj_event_mgr_t * p_manager)
{
    /* preconditions */
    assert(p_header_data != 00);
    assert(p_manager != 00);

    /* Size of this marker is fixed = 12 (we have already read marker and its size)*/
    if (p_header_size != 8) {
        opj_event_msg(p_manager, EVT_ERROR, "Error reading SOT marker\n");
        return OPJ_FALSE;
    }

    opj_read_bytes(p_header_data, p_tile_no, 2);    /* Isot */
    p_header_data += 2;
    opj_read_bytes(p_header_data, p_tot_len, 4);    /* Psot */
    p_header_data += 4;
    opj_read_bytes(p_header_data, p_current_part, 1); /* TPsot */
    ++p_header_data;
    opj_read_bytes(p_header_data, p_num_parts, 1);  /* TNsot */
    ++p_header_data;
    return OPJ_TRUE;
}