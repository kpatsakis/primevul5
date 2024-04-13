ConnStateData::splice()
{
    // normally we can splice here, because we just got client hello message

    if (fd_table[clientConnection->fd].ssl.get()) {
        // Restore default read methods
        fd_table[clientConnection->fd].read_method = &default_read_method;
        fd_table[clientConnection->fd].write_method = &default_write_method;
    }

    // XXX: assuming that there was an HTTP/1.1 CONNECT to begin with...
    // reset the current protocol to HTTP/1.1 (was "HTTPS" for the bumping process)
    transferProtocol = Http::ProtocolVersion();
    assert(!pipeline.empty());
    Http::StreamPointer context = pipeline.front();
    Must(context);
    Must(context->http);
    ClientHttpRequest *http = context->http;
    HttpRequest::Pointer request = http->request;
    context->finished();
    if (transparent()) {
        // For transparent connections, make a new fake CONNECT request, now
        // with SNI as target. doCallout() checks, adaptations may need that.
        return fakeAConnectRequest("splice", preservedClientData);
    } else {
        // For non transparent connections  make a new tunneled CONNECT, which
        // also sets the HttpRequest::flags::forceTunnel flag to avoid
        // respond with "Connection Established" to the client.
        // This fake CONNECT request required to allow use of SNI in
        // doCallout() checks and adaptations.
        return initiateTunneledRequest(request, Http::METHOD_CONNECT, "splice", preservedClientData);
    }
}