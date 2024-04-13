HttpStateData::statusIfComplete() const
{
    const HttpReply *rep = virginReply();
    /** \par
     * If the reply wants to close the connection, it takes precedence */

    if (httpHeaderHasConnDir(&rep->header, "close"))
        return COMPLETE_NONPERSISTENT_MSG;

    /** \par
     * If we didn't send a keep-alive request header, then this
     * can not be a persistent connection.
     */
    if (!flags.keepalive)
        return COMPLETE_NONPERSISTENT_MSG;

    /** \par
     * If we haven't sent the whole request then this can not be a persistent
     * connection.
     */
    if (!flags.request_sent) {
        debugs(11, 2, "Request not yet fully sent " << request->method << ' ' << entry->url());
        return COMPLETE_NONPERSISTENT_MSG;
    }

    /** \par
     * What does the reply have to say about keep-alive?
     */
    /**
     \bug XXX BUG?
     * If the origin server (HTTP/1.0) does not send a keep-alive
     * header, but keeps the connection open anyway, what happens?
     * We'll return here and http.c waits for an EOF before changing
     * store_status to STORE_OK.   Combine this with ENTRY_FWD_HDR_WAIT
     * and an error status code, and we might have to wait until
     * the server times out the socket.
     */
    if (!rep->keep_alive)
        return COMPLETE_NONPERSISTENT_MSG;

    return COMPLETE_PERSISTENT_MSG;
}