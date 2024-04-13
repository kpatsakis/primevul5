ConnStateData::postHttpsAccept()
{
    if (port->flags.tunnelSslBumping) {
#if USE_OPENSSL
        debugs(33, 5, "accept transparent connection: " << clientConnection);

        if (!Config.accessList.ssl_bump) {
            httpsSslBumpAccessCheckDone(ACCESS_DENIED, this);
            return;
        }

        MasterXaction::Pointer mx = new MasterXaction(XactionInitiator::initClient);
        mx->tcpClient = clientConnection;
        // Create a fake HTTP request for ssl_bump ACL check,
        // using tproxy/intercept provided destination IP and port.
        HttpRequest *request = new HttpRequest(mx);
        static char ip[MAX_IPSTRLEN];
        assert(clientConnection->flags & (COMM_TRANSPARENT | COMM_INTERCEPTION));
        request->url.host(clientConnection->local.toStr(ip, sizeof(ip)));
        request->url.port(clientConnection->local.port());
        request->myportname = port->name;

        ACLFilledChecklist *acl_checklist = new ACLFilledChecklist(Config.accessList.ssl_bump, request, NULL);
        acl_checklist->src_addr = clientConnection->remote;
        acl_checklist->my_addr = port->s;
        // Build a local AccessLogEntry to allow requiresAle() acls work
        acl_checklist->al = new AccessLogEntry;
        acl_checklist->al->cache.start_time = current_time;
        acl_checklist->al->tcpClient = clientConnection;
        acl_checklist->al->cache.port = port;
        acl_checklist->al->cache.caddr = log_addr;
        HTTPMSGUNLOCK(acl_checklist->al->request);
        acl_checklist->al->request = request;
        HTTPMSGLOCK(acl_checklist->al->request);
        Http::StreamPointer context = pipeline.front();
        ClientHttpRequest *http = context ? context->http : nullptr;
        const char *log_uri = http ? http->log_uri : nullptr;
        acl_checklist->syncAle(request, log_uri);
        acl_checklist->nonBlockingCheck(httpsSslBumpAccessCheckDone, this);
#else
        fatal("FATAL: SSL-Bump requires --with-openssl");
#endif
        return;
    } else {
        httpsEstablish(this, port->secure.staticContext);
    }
}