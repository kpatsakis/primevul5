static void opj_j2k_update_tlm(opj_j2k_t * p_j2k, OPJ_UINT32 p_tile_part_size)
{
    opj_write_bytes(p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_current,
                    p_j2k->m_current_tile_number, 1);           /* PSOT */
    ++p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_current;

    opj_write_bytes(p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_current,
                    p_tile_part_size, 4);                                       /* PSOT */
    p_j2k->m_specific_param.m_encoder.m_tlm_sot_offsets_current += 4;
}