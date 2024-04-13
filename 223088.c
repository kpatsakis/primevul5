static OPJ_FLOAT32 opj_j2k_get_tp_stride(opj_tcp_t * p_tcp)
{
    return (OPJ_FLOAT32)((p_tcp->m_nb_tile_parts - 1) * 14);
}