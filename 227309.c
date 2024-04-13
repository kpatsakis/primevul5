HttpStateData::buildRequestPrefix(MemBuf * mb)
{
    const int offset = mb->size;
    /* Uses a local httpver variable to print the HTTP label
     * since the HttpRequest may have an older version label.
     * XXX: This could create protocol bugs as the headers sent and
     * flow control should all be based on the HttpRequest version
     * not the one we are sending. Needs checking.
     */
    const AnyP::ProtocolVersion httpver = Http::ProtocolVersion();
    const SBuf url(_peer && !_peer->options.originserver ? request->effectiveRequestUri() : request->url.path());
    mb->appendf(SQUIDSBUFPH " " SQUIDSBUFPH " %s/%d.%d\r\n",
                SQUIDSBUFPRINT(request->method.image()),
                SQUIDSBUFPRINT(url),
                AnyP::ProtocolType_str[httpver.protocol],
                httpver.major,httpver.minor);
    /* build and pack headers */
    {
        HttpHeader hdr(hoRequest);
        httpBuildRequestHeader(request, entry, fwd->al, &hdr, flags);

        if (request->flags.pinned && request->flags.connectionAuth)
            request->flags.authSent = true;
        else if (hdr.has(Http::HdrType::AUTHORIZATION))
            request->flags.authSent = true;

        hdr.packInto(mb);
        hdr.clean();
    }
    /* append header terminator */
    mb->append(crlf, 2);
    return mb->size - offset;
}