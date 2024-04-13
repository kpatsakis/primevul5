static OPJ_BOOL opj_j2k_compare_qcc(opj_j2k_t *p_j2k,
                                    OPJ_UINT32 p_first_comp_no, OPJ_UINT32 p_second_comp_no)
{
    return opj_j2k_compare_SQcd_SQcc(p_j2k, p_j2k->m_current_tile_number,
                                     p_first_comp_no, p_second_comp_no);
}