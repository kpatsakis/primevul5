static const char* tcp_host_get_filter_type(hostlist_talker_t* host, conv_filter_type_e filter)
{
    if (filter == CONV_FT_SRC_PORT)
        return "tcp.srcport";

    if (filter == CONV_FT_DST_PORT)
        return "tcp.dstport";

    if (filter == CONV_FT_ANY_PORT)
        return "tcp.port";

    if(!host) {
        return CONV_FILTER_INVALID;
    }

    if (filter == CONV_FT_SRC_ADDRESS) {
        if (host->myaddress.type == AT_IPv4)
            return "ip.src";
        if (host->myaddress.type == AT_IPv6)
            return "ipv6.src";
    }

    if (filter == CONV_FT_DST_ADDRESS) {
        if (host->myaddress.type == AT_IPv4)
            return "ip.dst";
        if (host->myaddress.type == AT_IPv6)
            return "ipv6.dst";
    }

    if (filter == CONV_FT_ANY_ADDRESS) {
        if (host->myaddress.type == AT_IPv4)
            return "ip.addr";
        if (host->myaddress.type == AT_IPv6)
            return "ipv6.addr";
    }

    return CONV_FILTER_INVALID;
}