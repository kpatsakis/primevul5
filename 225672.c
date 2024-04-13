gchar* tcp_follow_address_filter(address* src_addr, address* dst_addr, int src_port, int dst_port)
{
    const gchar  *ip_version = src_addr->type == AT_IPv6 ? "v6" : "";
    gchar         src_addr_str[WS_INET6_ADDRSTRLEN];
    gchar         dst_addr_str[WS_INET6_ADDRSTRLEN];

    address_to_str_buf(src_addr, src_addr_str, sizeof(src_addr_str));
    address_to_str_buf(dst_addr, dst_addr_str, sizeof(dst_addr_str));

    return g_strdup_printf("((ip%s.src eq %s and tcp.srcport eq %d) and "
                     "(ip%s.dst eq %s and tcp.dstport eq %d))"
                     " or "
                     "((ip%s.src eq %s and tcp.srcport eq %d) and "
                     "(ip%s.dst eq %s and tcp.dstport eq %d))",
                     ip_version, src_addr_str, src_port,
                     ip_version, dst_addr_str, dst_port,
                     ip_version, dst_addr_str, dst_port,
                     ip_version, src_addr_str, src_port);

}