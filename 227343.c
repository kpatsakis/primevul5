ConnStateData::parseProxy1p0()
{
    ::Parser::Tokenizer tok(inBuf);
    tok.skip(Proxy1p0magic);

    // skip to first LF (assumes it is part of CRLF)
    static const CharacterSet lineContent = CharacterSet::LF.complement("non-LF");
    SBuf line;
    if (tok.prefix(line, lineContent, 107-Proxy1p0magic.length())) {
        if (tok.skip('\n')) {
            // found valid header
            inBuf = tok.remaining();
            needProxyProtocolHeader_ = false;
            // reset the tokenizer to work on found line only.
            tok.reset(line);
        } else
            return false; // no LF yet

    } else // protocol error only if there are more than 107 bytes prefix header
        return proxyProtocolError(inBuf.length() > 107? "PROXY/1.0 error: missing CRLF" : NULL);

    static const SBuf unknown("UNKNOWN"), tcpName("TCP");
    if (tok.skip(tcpName)) {

        // skip TCP/IP version number
        static const CharacterSet tcpVersions("TCP-version","46");
        if (!tok.skipOne(tcpVersions))
            return proxyProtocolError("PROXY/1.0 error: missing TCP version");

        // skip SP after protocol version
        if (!tok.skip(' '))
            return proxyProtocolError("PROXY/1.0 error: missing SP");

        SBuf ipa, ipb;
        int64_t porta, portb;
        static const CharacterSet ipChars = CharacterSet("IP Address",".:") + CharacterSet::HEXDIG;

        // parse:  src-IP SP dst-IP SP src-port SP dst-port CR
        // leave the LF until later.
        const bool correct = tok.prefix(ipa, ipChars) && tok.skip(' ') &&
                             tok.prefix(ipb, ipChars) && tok.skip(' ') &&
                             tok.int64(porta) && tok.skip(' ') &&
                             tok.int64(portb) &&
                             tok.skip('\r');
        if (!correct)
            return proxyProtocolError("PROXY/1.0 error: invalid syntax");

        // parse IP and port strings
        Ip::Address originalClient, originalDest;

        if (!originalClient.GetHostByName(ipa.c_str()))
            return proxyProtocolError("PROXY/1.0 error: invalid src-IP address");

        if (!originalDest.GetHostByName(ipb.c_str()))
            return proxyProtocolError("PROXY/1.0 error: invalid dst-IP address");

        if (porta > 0 && porta <= 0xFFFF) // max uint16_t
            originalClient.port(static_cast<uint16_t>(porta));
        else
            return proxyProtocolError("PROXY/1.0 error: invalid src port");

        if (portb > 0 && portb <= 0xFFFF) // max uint16_t
            originalDest.port(static_cast<uint16_t>(portb));
        else
            return proxyProtocolError("PROXY/1.0 error: invalid dst port");

        // we have original client and destination details now
        // replace the client connection values
        debugs(33, 5, "PROXY/1.0 protocol on connection " << clientConnection);
        clientConnection->local = originalDest;
        clientConnection->remote = originalClient;
        if ((clientConnection->flags & COMM_TRANSPARENT))
            clientConnection->flags ^= COMM_TRANSPARENT; // prevent TPROXY spoofing of this new IP.
        debugs(33, 5, "PROXY/1.0 upgrade: " << clientConnection);
        return true;

    } else if (tok.skip(unknown)) {
        // found valid but unusable header
        return true;

    } else
        return proxyProtocolError("PROXY/1.0 error: invalid protocol family");

    return false;
}