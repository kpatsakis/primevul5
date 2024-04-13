bool ConnStateData::serveDelayedError(Http::Stream *context)
{
    ClientHttpRequest *http = context->http;

    if (!sslServerBump)
        return false;

    assert(sslServerBump->entry);
    // Did we create an error entry while processing CONNECT?
    if (!sslServerBump->entry->isEmpty()) {
        quitAfterError(http->request);

        // Get the saved error entry and send it to the client by replacing the
        // ClientHttpRequest store entry with it.
        clientStreamNode *node = context->getClientReplyContext();
        clientReplyContext *repContext = dynamic_cast<clientReplyContext *>(node->data.getRaw());
        assert(repContext);
        debugs(33, 5, "Responding with delated error for " << http->uri);
        repContext->setReplyToStoreEntry(sslServerBump->entry, "delayed SslBump error");

        // Get error details from the fake certificate-peeking request.
        http->request->detailError(sslServerBump->request->errType, sslServerBump->request->errDetail);
        context->pullData();
        return true;
    }

    // In bump-server-first mode, we have not necessarily seen the intended
    // server name at certificate-peeking time. Check for domain mismatch now,
    // when we can extract the intended name from the bumped HTTP request.
    if (const Security::CertPointer &srvCert = sslServerBump->serverCert) {
        HttpRequest *request = http->request;
        if (!Ssl::checkX509ServerValidity(srvCert.get(), request->url.host())) {
            debugs(33, 2, "SQUID_X509_V_ERR_DOMAIN_MISMATCH: Certificate " <<
                   "does not match domainname " << request->url.host());

            bool allowDomainMismatch = false;
            if (Config.ssl_client.cert_error) {
                ACLFilledChecklist check(Config.ssl_client.cert_error, request, dash_str);
                check.al = http->al;
                check.sslErrors = new Security::CertErrors(Security::CertError(SQUID_X509_V_ERR_DOMAIN_MISMATCH, srvCert));
                check.syncAle(request, http->log_uri);
                allowDomainMismatch = check.fastCheck().allowed();
                delete check.sslErrors;
                check.sslErrors = NULL;
            }

            if (!allowDomainMismatch) {
                quitAfterError(request);

                clientStreamNode *node = context->getClientReplyContext();
                clientReplyContext *repContext = dynamic_cast<clientReplyContext *>(node->data.getRaw());
                assert (repContext);

                request->hier = sslServerBump->request->hier;

                // Create an error object and fill it
                ErrorState *err = new ErrorState(ERR_SECURE_CONNECT_FAIL, Http::scServiceUnavailable, request);
                err->src_addr = clientConnection->remote;
                Ssl::ErrorDetail *errDetail = new Ssl::ErrorDetail(
                    SQUID_X509_V_ERR_DOMAIN_MISMATCH,
                    srvCert.get(), nullptr);
                err->detail = errDetail;
                repContext->setReplyToError(request->method, err);
                assert(context->http->out.offset == 0);
                context->pullData();
                return true;
            }
        }
    }

    return false;
}