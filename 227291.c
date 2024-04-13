clientHttpConnectionsOpen(void)
{
    for (AnyP::PortCfgPointer s = HttpPortList; s != NULL; s = s->next) {
        const SBuf &scheme = AnyP::UriScheme(s->transport.protocol).image();

        if (MAXTCPLISTENPORTS == NHttpSockets) {
            debugs(1, DBG_IMPORTANT, "WARNING: You have too many '" << scheme << "_port' lines.");
            debugs(1, DBG_IMPORTANT, "         The limit is " << MAXTCPLISTENPORTS << " HTTP ports.");
            continue;
        }

#if USE_OPENSSL
        if (s->flags.tunnelSslBumping) {
            if (!Config.accessList.ssl_bump) {
                debugs(33, DBG_IMPORTANT, "WARNING: No ssl_bump configured. Disabling ssl-bump on " << scheme << "_port " << s->s);
                s->flags.tunnelSslBumping = false;
            }
            if (!s->secure.staticContext && !s->secure.generateHostCertificates) {
                debugs(1, DBG_IMPORTANT, "Will not bump SSL at " << scheme << "_port " << s->s << " due to TLS initialization failure.");
                s->flags.tunnelSslBumping = false;
                if (s->transport.protocol == AnyP::PROTO_HTTP)
                    s->secure.encryptTransport = false;
            }
            if (s->flags.tunnelSslBumping) {
                // Create ssl_ctx cache for this port.
                Ssl::TheGlobalContextStorage.addLocalStorage(s->s, s->secure.dynamicCertMemCacheSize);
            }
        }
#endif

        if (s->secure.encryptTransport && !s->secure.staticContext) {
            debugs(1, DBG_CRITICAL, "ERROR: Ignoring " << scheme << "_port " << s->s << " due to TLS context initialization failure.");
            continue;
        }

        // Fill out a Comm::Connection which IPC will open as a listener for us
        //  then pass back when active so we can start a TcpAcceptor subscription.
        s->listenConn = new Comm::Connection;
        s->listenConn->local = s->s;

        s->listenConn->flags = COMM_NONBLOCKING | (s->flags.tproxyIntercept ? COMM_TRANSPARENT : 0) |
                               (s->flags.natIntercept ? COMM_INTERCEPTION : 0);

        typedef CommCbFunPtrCallT<CommAcceptCbPtrFun> AcceptCall;
        if (s->transport.protocol == AnyP::PROTO_HTTP) {
            // setup the subscriptions such that new connections accepted by listenConn are handled by HTTP
            RefCount<AcceptCall> subCall = commCbCall(5, 5, "httpAccept", CommAcceptCbPtrFun(httpAccept, CommAcceptCbParams(NULL)));
            Subscription::Pointer sub = new CallSubscription<AcceptCall>(subCall);

            AsyncCall::Pointer listenCall = asyncCall(33,2, "clientListenerConnectionOpened",
                                            ListeningStartedDialer(&clientListenerConnectionOpened, s, Ipc::fdnHttpSocket, sub));
            Ipc::StartListening(SOCK_STREAM, IPPROTO_TCP, s->listenConn, Ipc::fdnHttpSocket, listenCall);

        } else if (s->transport.protocol == AnyP::PROTO_HTTPS) {
            // setup the subscriptions such that new connections accepted by listenConn are handled by HTTPS
            RefCount<AcceptCall> subCall = commCbCall(5, 5, "httpsAccept", CommAcceptCbPtrFun(httpsAccept, CommAcceptCbParams(NULL)));
            Subscription::Pointer sub = new CallSubscription<AcceptCall>(subCall);

            AsyncCall::Pointer listenCall = asyncCall(33, 2, "clientListenerConnectionOpened",
                                            ListeningStartedDialer(&clientListenerConnectionOpened,
                                                    s, Ipc::fdnHttpsSocket, sub));
            Ipc::StartListening(SOCK_STREAM, IPPROTO_TCP, s->listenConn, Ipc::fdnHttpsSocket, listenCall);
        }

        HttpSockets[NHttpSockets] = -1; // set in clientListenerConnectionOpened
        ++NHttpSockets;
    }
}