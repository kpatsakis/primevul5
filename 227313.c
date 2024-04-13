ConnStateData::unpinConnection(const bool andClose)
{
    debugs(33, 3, HERE << pinning.serverConnection);

    if (pinning.peer)
        cbdataReferenceDone(pinning.peer);

    if (Comm::IsConnOpen(pinning.serverConnection)) {
        if (pinning.closeHandler != NULL) {
            comm_remove_close_handler(pinning.serverConnection->fd, pinning.closeHandler);
            pinning.closeHandler = NULL;
        }

        stopPinnedConnectionMonitoring();

        // close the server side socket if requested
        if (andClose)
            pinning.serverConnection->close();
        pinning.serverConnection = NULL;
    }

    safe_free(pinning.host);

    pinning.zeroReply = false;

    /* NOTE: pinning.pinned should be kept. This combined with fd == -1 at the end of a request indicates that the host
     * connection has gone away */
}