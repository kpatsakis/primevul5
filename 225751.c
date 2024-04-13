tcp_build_filter(packet_info *pinfo)
{
    if( pinfo->net_src.type == AT_IPv4 && pinfo->net_dst.type == AT_IPv4 ) {
        /* TCP over IPv4 */
        return g_strdup_printf("(ip.addr eq %s and ip.addr eq %s) and (tcp.port eq %d and tcp.port eq %d)",
            address_to_str(pinfo->pool, &pinfo->net_src),
            address_to_str(pinfo->pool, &pinfo->net_dst),
            pinfo->srcport, pinfo->destport );
    }

    if( pinfo->net_src.type == AT_IPv6 && pinfo->net_dst.type == AT_IPv6 ) {
        /* TCP over IPv6 */
        return g_strdup_printf("(ipv6.addr eq %s and ipv6.addr eq %s) and (tcp.port eq %d and tcp.port eq %d)",
            address_to_str(pinfo->pool, &pinfo->net_src),
            address_to_str(pinfo->pool, &pinfo->net_dst),
            pinfo->srcport, pinfo->destport );
    }

    return NULL;
}