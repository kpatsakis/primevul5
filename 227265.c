HttpStateData::keepaliveAccounting(HttpReply *reply)
{
    if (flags.keepalive)
        if (_peer)
            ++ _peer->stats.n_keepalives_sent;

    if (reply->keep_alive) {
        if (_peer)
            ++ _peer->stats.n_keepalives_recv;

        if (Config.onoff.detect_broken_server_pconns
                && reply->bodySize(request->method) == -1 && !flags.chunked) {
            debugs(11, DBG_IMPORTANT, "keepaliveAccounting: Impossible keep-alive header from '" << entry->url() << "'" );
            // debugs(11, 2, "GOT HTTP REPLY HDR:\n---------\n" << readBuf->content() << "\n----------" );
            flags.keepalive_broken = true;
        }
    }
}