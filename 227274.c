ConnStateData::clientParseRequests()
{
    bool parsed_req = false;

    debugs(33, 5, HERE << clientConnection << ": attempting to parse");

    // Loop while we have read bytes that are not needed for producing the body
    // On errors, bodyPipe may become nil, but readMore will be cleared
    while (!inBuf.isEmpty() && !bodyPipe && flags.readMore) {

        // Prohibit concurrent requests when using a pinned to-server connection
        // because our Client classes do not support request pipelining.
        if (pinning.pinned && !pinning.readHandler) {
            debugs(33, 3, clientConnection << " waits for busy " << pinning.serverConnection);
            break;
        }

        /* Limit the number of concurrent requests */
        if (concurrentRequestQueueFilled())
            break;

        // try to parse the PROXY protocol header magic bytes
        if (needProxyProtocolHeader_) {
            if (!parseProxyProtocolHeader())
                break;

            // we have been waiting for PROXY to provide client-IP
            // for some lookups, ie rDNS and IDENT.
            whenClientIpKnown();

            // Done with PROXY protocol which has cleared preservingClientData_.
            // If the next protocol supports on_unsupported_protocol, then its
            // parseOneRequest() must reset preservingClientData_.
            assert(!preservingClientData_);
        }

        if (Http::StreamPointer context = parseOneRequest()) {
            debugs(33, 5, clientConnection << ": done parsing a request");

            AsyncCall::Pointer timeoutCall = commCbCall(5, 4, "clientLifetimeTimeout",
                                             CommTimeoutCbPtrFun(clientLifetimeTimeout, context->http));
            commSetConnTimeout(clientConnection, Config.Timeout.lifetime, timeoutCall);

            context->registerWithConn();

#if USE_OPENSSL
            if (switchedToHttps())
                parsedBumpedRequestCount++;
#endif

            processParsedRequest(context);

            parsed_req = true; // XXX: do we really need to parse everything right NOW ?

            if (context->mayUseConnection()) {
                debugs(33, 3, HERE << "Not parsing new requests, as this request may need the connection");
                break;
            }
        } else {
            debugs(33, 5, clientConnection << ": not enough request data: " <<
                   inBuf.length() << " < " << Config.maxRequestHeaderSize);
            Must(inBuf.length() < Config.maxRequestHeaderSize);
            break;
        }
    }

    /* XXX where to 'finish' the parsing pass? */
    return parsed_req;
}