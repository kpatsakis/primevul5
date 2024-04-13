ConnStateData::parseProxy2p0()
{
    static const SBuf::size_type prefixLen = Proxy2p0magic.length();
    if (inBuf.length() < prefixLen + 4)
        return false; // need more bytes

    if ((inBuf[prefixLen] & 0xF0) != 0x20) // version == 2 is mandatory
        return proxyProtocolError("PROXY/2.0 error: invalid version");

    const char command = (inBuf[prefixLen] & 0x0F);
    if ((command & 0xFE) != 0x00) // values other than 0x0-0x1 are invalid
        return proxyProtocolError("PROXY/2.0 error: invalid command");

    const char family = (inBuf[prefixLen+1] & 0xF0) >>4;
    if (family > 0x3) // values other than 0x0-0x3 are invalid
        return proxyProtocolError("PROXY/2.0 error: invalid family");

    const char proto = (inBuf[prefixLen+1] & 0x0F);
    if (proto > 0x2) // values other than 0x0-0x2 are invalid
        return proxyProtocolError("PROXY/2.0 error: invalid protocol type");

    const char *clen = inBuf.rawContent() + prefixLen + 2;
    uint16_t len;
    memcpy(&len, clen, sizeof(len));
    len = ntohs(len);

    if (inBuf.length() < prefixLen + 4 + len)
        return false; // need more bytes

    inBuf.consume(prefixLen + 4); // 4 being the extra bytes
    const SBuf extra = inBuf.consume(len);
    needProxyProtocolHeader_ = false; // found successfully

    // LOCAL connections do nothing with the extras
    if (command == 0x00/* LOCAL*/)
        return true;

    union pax {
        struct {        /* for TCP/UDP over IPv4, len = 12 */
            struct in_addr src_addr;
            struct in_addr dst_addr;
            uint16_t src_port;
            uint16_t dst_port;
        } ipv4_addr;
        struct {        /* for TCP/UDP over IPv6, len = 36 */
            struct in6_addr src_addr;
            struct in6_addr dst_addr;
            uint16_t src_port;
            uint16_t dst_port;
        } ipv6_addr;
#if NOT_SUPPORTED
        struct {        /* for AF_UNIX sockets, len = 216 */
            uint8_t src_addr[108];
            uint8_t dst_addr[108];
        } unix_addr;
#endif
    };

    pax ipu;
    memcpy(&ipu, extra.rawContent(), sizeof(pax));

    // replace the client connection values
    debugs(33, 5, "PROXY/2.0 protocol on connection " << clientConnection);
    switch (family) {
    case 0x1: // IPv4
        clientConnection->local = ipu.ipv4_addr.dst_addr;
        clientConnection->local.port(ntohs(ipu.ipv4_addr.dst_port));
        clientConnection->remote = ipu.ipv4_addr.src_addr;
        clientConnection->remote.port(ntohs(ipu.ipv4_addr.src_port));
        if ((clientConnection->flags & COMM_TRANSPARENT))
            clientConnection->flags ^= COMM_TRANSPARENT; // prevent TPROXY spoofing of this new IP.
        break;
    case 0x2: // IPv6
        clientConnection->local = ipu.ipv6_addr.dst_addr;
        clientConnection->local.port(ntohs(ipu.ipv6_addr.dst_port));
        clientConnection->remote = ipu.ipv6_addr.src_addr;
        clientConnection->remote.port(ntohs(ipu.ipv6_addr.src_port));
        if ((clientConnection->flags & COMM_TRANSPARENT))
            clientConnection->flags ^= COMM_TRANSPARENT; // prevent TPROXY spoofing of this new IP.
        break;
    default: // do nothing
        break;
    }
    debugs(33, 5, "PROXY/2.0 upgrade: " << clientConnection);
    return true;
}