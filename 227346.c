HttpStateData::HttpStateData(FwdState *theFwdState) :
    AsyncJob("HttpStateData"),
    Client(theFwdState),
    lastChunk(0),
    httpChunkDecoder(NULL),
    payloadSeen(0),
    payloadTruncated(0),
    sawDateGoBack(false)
{
    debugs(11,5,HERE << "HttpStateData " << this << " created");
    ignoreCacheControl = false;
    surrogateNoStore = false;
    serverConnection = fwd->serverConnection();

    if (fwd->serverConnection() != NULL)
        _peer = cbdataReference(fwd->serverConnection()->getPeer());         /* might be NULL */

    if (_peer) {
        request->flags.proxying = true;
        /*
         * This NEIGHBOR_PROXY_ONLY check probably shouldn't be here.
         * We might end up getting the object from somewhere else if,
         * for example, the request to this neighbor fails.
         */
        if (_peer->options.proxy_only)
            entry->releaseRequest(true);

#if USE_DELAY_POOLS
        entry->setNoDelay(_peer->options.no_delay);
#endif
    }

    /*
     * register the handler to free HTTP state data when the FD closes
     */
    typedef CommCbMemFunT<HttpStateData, CommCloseCbParams> Dialer;
    closeHandler = JobCallback(9, 5, Dialer, this, HttpStateData::httpStateConnClosed);
    comm_add_close_handler(serverConnection->fd, closeHandler);
}