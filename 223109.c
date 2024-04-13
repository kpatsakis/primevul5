static void opj_j2k_tcp_data_destroy(opj_tcp_t *p_tcp)
{
    if (p_tcp->m_data) {
        opj_free(p_tcp->m_data);
        p_tcp->m_data = NULL;
        p_tcp->m_data_size = 0;
    }
}