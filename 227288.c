ConnStateData::afterClientRead()
{
#if USE_OPENSSL
    if (parsingTlsHandshake) {
        parseTlsHandshake();
        return;
    }
#endif

    /* Process next request */
    if (pipeline.empty())
        fd_note(clientConnection->fd, "Reading next request");

    if (!clientParseRequests()) {
        if (!isOpen())
            return;
        /*
         * If the client here is half closed and we failed
         * to parse a request, close the connection.
         * The above check with connFinishedWithConn() only
         * succeeds _if_ the buffer is empty which it won't
         * be if we have an incomplete request.
         * XXX: This duplicates ConnStateData::kick
         */
        if (pipeline.empty() && commIsHalfClosed(clientConnection->fd)) {
            debugs(33, 5, clientConnection << ": half-closed connection, no completed request parsed, connection closing.");
            clientConnection->close();
            return;
        }
    }

    if (!isOpen())
        return;

    clientAfterReadingRequests();
}