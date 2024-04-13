ConnStateData::startPeekAndSplice()
{
    // This is the Step2 of the SSL bumping
    assert(sslServerBump);
    Http::StreamPointer context = pipeline.front();
    ClientHttpRequest *http = context ? context->http : nullptr;

    if (sslServerBump->step == Ssl::bumpStep1) {
        sslServerBump->step = Ssl::bumpStep2;
        // Run a accessList check to check if want to splice or continue bumping

        ACLFilledChecklist *acl_checklist = new ACLFilledChecklist(Config.accessList.ssl_bump, sslServerBump->request.getRaw(), nullptr);
        acl_checklist->al = http ? http->al : nullptr;
        //acl_checklist->src_addr = params.conn->remote;
        //acl_checklist->my_addr = s->s;
        acl_checklist->banAction(allow_t(ACCESS_ALLOWED, Ssl::bumpNone));
        acl_checklist->banAction(allow_t(ACCESS_ALLOWED, Ssl::bumpClientFirst));
        acl_checklist->banAction(allow_t(ACCESS_ALLOWED, Ssl::bumpServerFirst));
        const char *log_uri = http ? http->log_uri : nullptr;
        acl_checklist->syncAle(sslServerBump->request.getRaw(), log_uri);
        acl_checklist->nonBlockingCheck(httpsSslBumpStep2AccessCheckDone, this);
        return;
    }

    // will call httpsPeeked() with certificate and connection, eventually
    Security::ContextPointer unConfiguredCTX(Ssl::createSSLContext(port->secure.signingCa.cert, port->secure.signingCa.pkey, port->secure));
    fd_table[clientConnection->fd].dynamicTlsContext = unConfiguredCTX;

    if (!httpsCreate(this, unConfiguredCTX))
        return;

    switchedToHttps_ = true;

    auto ssl = fd_table[clientConnection->fd].ssl.get();
    BIO *b = SSL_get_rbio(ssl);
    Ssl::ClientBio *bio = static_cast<Ssl::ClientBio *>(BIO_get_data(b));
    bio->setReadBufData(inBuf);
    bio->hold(true);

    // Here squid should have all of the client hello message so the
    // tlsAttemptHandshake() should return 0.
    // This block exist only to force openSSL parse client hello and detect
    // ERR_SECURE_ACCEPT_FAIL error, which should be checked and splice if required.
    if (tlsAttemptHandshake(this, nullptr) < 0) {
        debugs(83, 2, "TLS handshake failed.");
        HttpRequest::Pointer request(http ? http->request : nullptr);
        if (!clientTunnelOnError(this, context, request, HttpRequestMethod(), ERR_SECURE_ACCEPT_FAIL))
            clientConnection->close();
        return;
    }

    // We need to reset inBuf here, to be used by incoming requests in the case
    // of SSL bump
    inBuf.clear();

    debugs(83, 5, "Peek and splice at step2 done. Start forwarding the request!!! ");
    FwdState::Start(clientConnection, sslServerBump->entry, sslServerBump->request.getRaw(), http ? http->al : NULL);
}