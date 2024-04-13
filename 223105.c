static OPJ_BOOL opj_j2k_write_plt_in_memory(opj_j2k_t *p_j2k,
        opj_tcd_marker_info_t* marker_info,
        OPJ_BYTE * p_data,
        OPJ_UINT32 * p_data_written,
        opj_event_mgr_t * p_manager)
{
    OPJ_BYTE Zplt = 0;
    OPJ_UINT16 Lplt;
    OPJ_BYTE* p_data_start = p_data;
    OPJ_BYTE* p_data_Lplt = p_data + 2;
    OPJ_UINT32 i;

    OPJ_UNUSED(p_j2k);

    opj_write_bytes(p_data, J2K_MS_PLT, 2);
    p_data += 2;

    /* Reserve space for Lplt */
    p_data += 2;

    opj_write_bytes(p_data, Zplt, 1);
    p_data += 1;

    Lplt = 3;

    for (i = 0; i < marker_info->packet_count; i++) {
        OPJ_BYTE var_bytes[5];
        OPJ_UINT8 var_bytes_size = 0;
        OPJ_UINT32 packet_size = marker_info->p_packet_size[i];

        /* Packet size written in variable-length way, starting with LSB */
        var_bytes[var_bytes_size] = (OPJ_BYTE)(packet_size & 0x7f);
        var_bytes_size ++;
        packet_size >>= 7;
        while (packet_size > 0) {
            var_bytes[var_bytes_size] = (OPJ_BYTE)((packet_size & 0x7f) | 0x80);
            var_bytes_size ++;
            packet_size >>= 7;
        }

        /* Check if that can fit in the current PLT marker. If not, finish */
        /* current one, and start a new one */
        if (Lplt + var_bytes_size > 65535) {
            if (Zplt == 255) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "More than 255 PLT markers would be needed for current tile-part !\n");
                return OPJ_FALSE;
            }

            /* Patch Lplt */
            opj_write_bytes(p_data_Lplt, Lplt, 2);

            /* Start new segment */
            opj_write_bytes(p_data, J2K_MS_PLT, 2);
            p_data += 2;

            /* Reserve space for Lplt */
            p_data_Lplt = p_data;
            p_data += 2;

            Zplt ++;
            opj_write_bytes(p_data, Zplt, 1);
            p_data += 1;

            Lplt = 3;
        }

        Lplt = (OPJ_UINT16)(Lplt + var_bytes_size);

        /* Serialize variable-length packet size, starting with MSB */
        for (; var_bytes_size > 0; --var_bytes_size) {
            opj_write_bytes(p_data, var_bytes[var_bytes_size - 1], 1);
            p_data += 1;
        }
    }

    *p_data_written = (OPJ_UINT32)(p_data - p_data_start);

    /* Patch Lplt */
    opj_write_bytes(p_data_Lplt, Lplt, 2);

    return OPJ_TRUE;
}