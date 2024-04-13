HttpStateData::sendRequest()
{
    MemBuf mb;

    debugs(11, 5, HERE << serverConnection << ", request " << request << ", this " << this << ".");

    if (!Comm::IsConnOpen(serverConnection)) {
        debugs(11,3, HERE << "cannot send request to closing " << serverConnection);
        assert(closeHandler != NULL);
        return false;
    }

    typedef CommCbMemFunT<HttpStateData, CommTimeoutCbParams> TimeoutDialer;
    AsyncCall::Pointer timeoutCall =  JobCallback(11, 5,
                                      TimeoutDialer, this, HttpStateData::httpTimeout);
    commSetConnTimeout(serverConnection, Config.Timeout.lifetime, timeoutCall);
    flags.do_next_read = true;
    maybeReadVirginBody();

    if (request->body_pipe != NULL) {
        if (!startRequestBodyFlow()) // register to receive body data
            return false;
        typedef CommCbMemFunT<HttpStateData, CommIoCbParams> Dialer;
        requestSender = JobCallback(11,5,
                                    Dialer, this, HttpStateData::sentRequestBody);

        Must(!flags.chunked_request);
        // use chunked encoding if we do not know the length
        if (request->content_length < 0)
            flags.chunked_request = true;
    } else {
        assert(!requestBodySource);
        typedef CommCbMemFunT<HttpStateData, CommIoCbParams> Dialer;
        requestSender = JobCallback(11,5,
                                    Dialer, this,  HttpStateData::wroteLast);
    }

    flags.originpeer = (_peer != NULL && _peer->options.originserver);
    flags.proxying = (_peer != NULL && !flags.originpeer);

    /*
     * Is keep-alive okay for all request methods?
     */
    if (request->flags.mustKeepalive)
        flags.keepalive = true;
    else if (request->flags.pinned)
        flags.keepalive = request->persistent();
    else if (!Config.onoff.server_pconns)
        flags.keepalive = false;
    else if (_peer == NULL)
        flags.keepalive = true;
    else if (_peer->stats.n_keepalives_sent < 10)
        flags.keepalive = true;
    else if ((double) _peer->stats.n_keepalives_recv /
             (double) _peer->stats.n_keepalives_sent > 0.50)
        flags.keepalive = true;

    if (_peer) {
        /*The old code here was
          if (neighborType(_peer, request->url) == PEER_SIBLING && ...
          which is equivalent to:
          if (neighborType(_peer, URL()) == PEER_SIBLING && ...
          or better:
          if (((_peer->type == PEER_MULTICAST && p->options.mcast_siblings) ||
                 _peer->type == PEER_SIBLINGS ) && _peer->options.allow_miss)
               flags.only_if_cached = 1;

           But I suppose it was a bug
         */
        if (neighborType(_peer, request->url) == PEER_SIBLING && !_peer->options.allow_miss)
            flags.only_if_cached = true;

        flags.front_end_https = _peer->front_end_https;
    }

    mb.init();
    request->peer_host=_peer?_peer->host:NULL;
    buildRequestPrefix(&mb);

    debugs(11, 2, "HTTP Server " << serverConnection);
    debugs(11, 2, "HTTP Server REQUEST:\n---------\n" << mb.buf << "\n----------");

    Comm::Write(serverConnection, &mb, requestSender);
    return true;
}