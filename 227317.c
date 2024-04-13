ConnStateData::whenClientIpKnown()
{
    if (Config.onoff.log_fqdn)
        fqdncache_gethostbyaddr(clientConnection->remote, FQDN_LOOKUP_IF_MISS);

#if USE_IDENT
    if (Ident::TheConfig.identLookup) {
        ACLFilledChecklist identChecklist(Ident::TheConfig.identLookup, NULL, NULL);
        identChecklist.src_addr = clientConnection->remote;
        identChecklist.my_addr = clientConnection->local;
        if (identChecklist.fastCheck().allowed())
            Ident::Start(clientConnection, clientIdentDone, this);
    }
#endif

    clientdbEstablished(clientConnection->remote, 1);

#if USE_DELAY_POOLS
    fd_table[clientConnection->fd].clientInfo = NULL;

    if (!Config.onoff.client_db)
        return; // client delay pools require client_db

    ClientDelayPools& pools(Config.ClientDelay.pools);
    if (pools.size()) {
        ACLFilledChecklist ch(NULL, NULL, NULL);

        // TODO: we check early to limit error response bandwith but we
        // should recheck when we can honor delay_pool_uses_indirect
        // TODO: we should also pass the port details for myportname here.
        ch.src_addr = clientConnection->remote;
        ch.my_addr = clientConnection->local;

        for (unsigned int pool = 0; pool < pools.size(); ++pool) {

            /* pools require explicit 'allow' to assign a client into them */
            if (pools[pool].access) {
                ch.changeAcl(pools[pool].access);
                allow_t answer = ch.fastCheck();
                if (answer.allowed()) {

                    /*  request client information from db after we did all checks
                        this will save hash lookup if client failed checks */
                    ClientInfo * cli = clientdbGetInfo(clientConnection->remote);
                    assert(cli);

                    /* put client info in FDE */
                    fd_table[clientConnection->fd].clientInfo = cli;

                    /* setup write limiter for this request */
                    const double burst = floor(0.5 +
                                               (pools[pool].highwatermark * Config.ClientDelay.initial)/100.0);
                    cli->setWriteLimiter(pools[pool].rate, burst, pools[pool].highwatermark);
                    break;
                } else {
                    debugs(83, 4, HERE << "Delay pool " << pool << " skipped because ACL " << answer);
                }
            }
        }
    }
#endif

    // kids must extend to actually start doing something (e.g., reading)
}