ClientHttpRequest::logRequest()
{
    if (!out.size && logType.oldType == LOG_TAG_NONE)
        debugs(33, 5, "logging half-baked transaction: " << log_uri);

    al->icp.opcode = ICP_INVALID;
    al->url = log_uri;
    debugs(33, 9, "clientLogRequest: al.url='" << al->url << "'");

    if (al->reply) {
        al->http.code = al->reply->sline.status();
        al->http.content_type = al->reply->content_type.termedBuf();
    } else if (loggingEntry() && loggingEntry()->mem_obj) {
        al->http.code = loggingEntry()->mem_obj->getReply()->sline.status();
        al->http.content_type = loggingEntry()->mem_obj->getReply()->content_type.termedBuf();
    }

    debugs(33, 9, "clientLogRequest: http.code='" << al->http.code << "'");

    if (loggingEntry() && loggingEntry()->mem_obj && loggingEntry()->objectLen() >= 0)
        al->cache.objectSize = loggingEntry()->contentLen(); // payload duplicate ?? with or without TE ?

    al->http.clientRequestSz.header = req_sz;
    // the virgin request is saved to al->request
    if (al->request && al->request->body_pipe)
        al->http.clientRequestSz.payloadData = al->request->body_pipe->producedSize();
    al->http.clientReplySz.header = out.headers_sz;
    // XXX: calculate without payload encoding or headers !!
    al->http.clientReplySz.payloadData = out.size - out.headers_sz; // pretend its all un-encoded data for now.

    al->cache.highOffset = out.offset;

    al->cache.code = logType;

    tvSub(al->cache.trTime, al->cache.start_time, current_time);

    if (request)
        prepareLogWithRequestDetails(request, al);

#if USE_OPENSSL && 0

    /* This is broken. Fails if the connection has been closed. Needs
     * to snarf the ssl details some place earlier..
     */
    if (getConn() != NULL)
        al->cache.ssluser = sslGetUserEmail(fd_table[getConn()->fd].ssl);

#endif

    /* Add notes (if we have a request to annotate) */
    if (request) {
        // The al->notes and request->notes must point to the same object.
        (void)SyncNotes(*al, *request);
        for (auto i = Config.notes.begin(); i != Config.notes.end(); ++i) {
            if (const char *value = (*i)->match(request, al->reply, al)) {
                NotePairs &notes = SyncNotes(*al, *request);
                notes.add((*i)->key.termedBuf(), value);
                debugs(33, 3, (*i)->key.termedBuf() << " " << value);
            }
        }
    }

    ACLFilledChecklist checklist(NULL, request, NULL);
    if (al->reply) {
        checklist.reply = al->reply;
        HTTPMSGLOCK(checklist.reply);
    }

    if (request) {
        HTTPMSGUNLOCK(al->adapted_request);
        al->adapted_request = request;
        HTTPMSGLOCK(al->adapted_request);
    }
    // no need checklist.syncAle(): already synced
    checklist.al = al;
    accessLogLog(al, &checklist);

    bool updatePerformanceCounters = true;
    if (Config.accessList.stats_collection) {
        ACLFilledChecklist statsCheck(Config.accessList.stats_collection, request, NULL);
        statsCheck.al = al;
        if (al->reply) {
            statsCheck.reply = al->reply;
            HTTPMSGLOCK(statsCheck.reply);
        }
        updatePerformanceCounters = statsCheck.fastCheck().allowed();
    }

    if (updatePerformanceCounters) {
        if (request)
            updateCounters();

        if (getConn() != NULL && getConn()->clientConnection != NULL)
            clientdbUpdate(getConn()->clientConnection->remote, logType, AnyP::PROTO_HTTP, out.size);
    }
}