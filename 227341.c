ConnStateData::initiateTunneledRequest(HttpRequest::Pointer const &cause, Http::MethodType const method, const char *reason, const SBuf &payload)
{
    // fake a CONNECT request to force connState to tunnel
    SBuf connectHost;
    unsigned short connectPort = 0;

    if (pinning.serverConnection != nullptr) {
        static char ip[MAX_IPSTRLEN];
        connectHost = pinning.serverConnection->remote.toStr(ip, sizeof(ip));
        connectPort = pinning.serverConnection->remote.port();
    } else if (cause) {
        connectHost = cause->url.hostOrIp();
        connectPort = cause->url.port();
#if USE_OPENSSL
    } else if (!tlsConnectHostOrIp.isEmpty()) {
        connectHost = tlsConnectHostOrIp;
        connectPort = tlsConnectPort;
#endif
    } else if (transparent()) {
        static char ip[MAX_IPSTRLEN];
        connectHost = clientConnection->local.toStr(ip, sizeof(ip));
        connectPort = clientConnection->local.port();
    } else {
        debugs(33, 2, "Not able to compute URL, abort request tunneling for " << reason);
        return false;
    }

    debugs(33, 2, "Request tunneling for " << reason);
    ClientHttpRequest *http = buildFakeRequest(method, connectHost, connectPort, payload);
    HttpRequest::Pointer request = http->request;
    request->flags.forceTunnel = true;
    http->calloutContext = new ClientRequestContext(http);
    http->doCallouts();
    clientProcessRequestFinished(this, request);
    return true;
}