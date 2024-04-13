gchar* tcp_follow_index_filter(int stream)
{
    return g_strdup_printf("tcp.stream eq %d", stream);
}