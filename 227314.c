HttpStateData::processReplyHeader()
{
    /** Creates a blank header. If this routine is made incremental, this will not do */

    /* NP: all exit points to this function MUST call ctx_exit(ctx) */
    Ctx ctx = ctx_enter(entry->mem_obj->urlXXX());

    debugs(11, 3, "processReplyHeader: key '" << entry->getMD5Text() << "'");

    assert(!flags.headers_parsed);

    if (!inBuf.length()) {
        ctx_exit(ctx);
        return;
    }

    /* Attempt to parse the first line; this will define where the protocol, status, reason-phrase and header begin */
    {
        if (hp == NULL)
            hp = new Http1::ResponseParser;

        bool parsedOk = hp->parse(inBuf);

        // sync the buffers after parsing.
        inBuf = hp->remaining();

        if (hp->needsMoreData()) {
            if (eof) { // no more data coming
                /* Bug 2879: Replies may terminate with \r\n then EOF instead of \r\n\r\n.
                 * We also may receive truncated responses.
                 * Ensure here that we have at minimum two \r\n when EOF is seen.
                 */
                inBuf.append("\r\n\r\n", 4);
                // retry the parse
                parsedOk = hp->parse(inBuf);
                // sync the buffers after parsing.
                inBuf = hp->remaining();
            } else {
                debugs(33, 5, "Incomplete response, waiting for end of response headers");
                ctx_exit(ctx);
                return;
            }
        }

        if (!parsedOk) {
            // unrecoverable parsing error
            // TODO: Use Raw! XXX: inBuf no longer has the [beginning of the] malformed header.
            debugs(11, 3, "Non-HTTP-compliant header:\n---------\n" << inBuf << "\n----------");
            flags.headers_parsed = true;
            HttpReply *newrep = new HttpReply;
            newrep->sline.set(Http::ProtocolVersion(), hp->parseStatusCode);
            setVirginReply(newrep);
            ctx_exit(ctx);
            return;
        }
    }

    /* We know the whole response is in parser now */
    debugs(11, 2, "HTTP Server " << serverConnection);
    debugs(11, 2, "HTTP Server RESPONSE:\n---------\n" <<
           hp->messageProtocol() << " " << hp->messageStatus() << " " << hp->reasonPhrase() << "\n" <<
           hp->mimeHeader() <<
           "----------");

    // reset payload tracking to begin after message headers
    payloadSeen = inBuf.length();

    HttpReply *newrep = new HttpReply;
    // XXX: RFC 7230 indicates we MAY ignore the reason phrase,
    //      and use an empty string on unknown status.
    //      We do that now to avoid performance regression from using SBuf::c_str()
    newrep->sline.set(Http::ProtocolVersion(1,1), hp->messageStatus() /* , hp->reasonPhrase() */);
    newrep->sline.protocol = newrep->sline.version.protocol = hp->messageProtocol().protocol;
    newrep->sline.version.major = hp->messageProtocol().major;
    newrep->sline.version.minor = hp->messageProtocol().minor;

    // parse headers
    if (!newrep->parseHeader(*hp)) {
        // XXX: when Http::ProtocolVersion is a function, remove this hack. just set with messageProtocol()
        newrep->sline.set(Http::ProtocolVersion(), Http::scInvalidHeader);
        newrep->sline.version.protocol = hp->messageProtocol().protocol;
        newrep->sline.version.major = hp->messageProtocol().major;
        newrep->sline.version.minor = hp->messageProtocol().minor;
        debugs(11, 2, "error parsing response headers mime block");
    }

    // done with Parser, now process using the HttpReply
    hp = NULL;

    newrep->sources |= request->url.getScheme() == AnyP::PROTO_HTTPS ? HttpMsg::srcHttps : HttpMsg::srcHttp;

    newrep->removeStaleWarnings();

    if (newrep->sline.protocol == AnyP::PROTO_HTTP && newrep->sline.status() >= 100 && newrep->sline.status() < 200) {
        handle1xx(newrep);
        ctx_exit(ctx);
        return;
    }

    flags.chunked = false;
    if (newrep->sline.protocol == AnyP::PROTO_HTTP && newrep->header.chunked()) {
        flags.chunked = true;
        httpChunkDecoder = new Http1::TeChunkedParser;
    }

    if (!peerSupportsConnectionPinning())
        request->flags.connectionAuthDisabled = true;

    HttpReply *vrep = setVirginReply(newrep);
    flags.headers_parsed = true;

    keepaliveAccounting(vrep);

    checkDateSkew(vrep);

    processSurrogateControl (vrep);

    request->hier.peer_reply_status = newrep->sline.status();

    ctx_exit(ctx);
}