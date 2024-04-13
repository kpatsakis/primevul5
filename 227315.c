ConnStateData::getSslContextStart()
{
    // If we are called, then CONNECT has succeeded. Finalize it.
    if (auto xact = pipeline.front()) {
        if (xact->http && xact->http->request && xact->http->request->method == Http::METHOD_CONNECT)
            xact->finished();
        // cannot proceed with encryption if requests wait for plain responses
        Must(pipeline.empty());
    }
    /* careful: finished() above frees request, host, etc. */

    if (port->secure.generateHostCertificates) {
        Ssl::CertificateProperties certProperties;
        buildSslCertGenerationParams(certProperties);

        // Disable caching for bumpPeekAndSplice mode
        if (!(sslServerBump && (sslServerBump->act.step1 == Ssl::bumpPeek || sslServerBump->act.step1 == Ssl::bumpStare))) {
            sslBumpCertKey.clear();
            Ssl::InRamCertificateDbKey(certProperties, sslBumpCertKey);
            assert(!sslBumpCertKey.isEmpty());

            Security::ContextPointer ctx(getTlsContextFromCache(sslBumpCertKey, certProperties));
            if (ctx) {
                getSslContextDone(ctx);
                return;
            }
        }

#if USE_SSL_CRTD
        try {
            debugs(33, 5, HERE << "Generating SSL certificate for " << certProperties.commonName << " using ssl_crtd.");
            Ssl::CrtdMessage request_message(Ssl::CrtdMessage::REQUEST);
            request_message.setCode(Ssl::CrtdMessage::code_new_certificate);
            request_message.composeRequest(certProperties);
            debugs(33, 5, HERE << "SSL crtd request: " << request_message.compose().c_str());
            Ssl::Helper::Submit(request_message, sslCrtdHandleReplyWrapper, this);
            return;
        } catch (const std::exception &e) {
            debugs(33, DBG_IMPORTANT, "ERROR: Failed to compose ssl_crtd " <<
                   "request for " << certProperties.commonName <<
                   " certificate: " << e.what() << "; will now block to " <<
                   "generate that certificate.");
            // fall through to do blocking in-process generation.
        }
#endif // USE_SSL_CRTD

        debugs(33, 5, HERE << "Generating SSL certificate for " << certProperties.commonName);
        if (sslServerBump && (sslServerBump->act.step1 == Ssl::bumpPeek || sslServerBump->act.step1 == Ssl::bumpStare)) {
            doPeekAndSpliceStep();
            auto ssl = fd_table[clientConnection->fd].ssl.get();
            if (!Ssl::configureSSL(ssl, certProperties, *port))
                debugs(33, 5, "Failed to set certificates to ssl object for PeekAndSplice mode");

            Security::ContextPointer ctx(Security::GetFrom(fd_table[clientConnection->fd].ssl));
            Ssl::configureUnconfiguredSslContext(ctx, certProperties.signAlgorithm, *port);
        } else {
            Security::ContextPointer dynCtx(Ssl::GenerateSslContext(certProperties, port->secure, (signAlgorithm == Ssl::algSignTrusted)));
            if (dynCtx && !sslBumpCertKey.isEmpty())
                storeTlsContextToCache(sslBumpCertKey, dynCtx);
            getSslContextDone(dynCtx);
        }
        return;
    }

    Security::ContextPointer nil;
    getSslContextDone(nil);
}