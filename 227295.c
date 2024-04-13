ConnStateData::ConnStateData(const MasterXaction::Pointer &xact) :
    AsyncJob("ConnStateData"), // kids overwrite
    Server(xact),
    bodyParser(nullptr),
#if USE_OPENSSL
    sslBumpMode(Ssl::bumpEnd),
    tlsParser(Security::HandshakeParser::fromClient),
#endif
    needProxyProtocolHeader_(false),
#if USE_OPENSSL
    switchedToHttps_(false),
    parsingTlsHandshake(false),
    parsedBumpedRequestCount(0),
    tlsConnectPort(0),
    sslServerBump(NULL),
    signAlgorithm(Ssl::algSignTrusted),
#endif
    stoppedSending_(NULL),
    stoppedReceiving_(NULL)
{
    flags.readMore = true; // kids may overwrite
    flags.swanSang = false;

    pinning.host = NULL;
    pinning.port = -1;
    pinning.pinned = false;
    pinning.auth = false;
    pinning.zeroReply = false;
    pinning.peer = NULL;

    // store the details required for creating more MasterXaction objects as new requests come in
    log_addr = xact->tcpClient->remote;
    log_addr.applyMask(Config.Addrs.client_netmask);

    // register to receive notice of Squid signal events
    // which may affect long persisting client connections
    registerRunner();
}