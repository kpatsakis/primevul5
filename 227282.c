ConnStateData::validatePinnedConnection(HttpRequest *request, const CachePeer *aPeer)
{
    debugs(33, 7, HERE << pinning.serverConnection);

    bool valid = true;
    if (!Comm::IsConnOpen(pinning.serverConnection))
        valid = false;
    else if (pinning.auth && pinning.host && request && strcasecmp(pinning.host, request->url.host()) != 0)
        valid = false;
    else if (request && pinning.port != request->url.port())
        valid = false;
    else if (pinning.peer && !cbdataReferenceValid(pinning.peer))
        valid = false;
    else if (aPeer != pinning.peer)
        valid = false;

    if (!valid) {
        /* The pinning info is not safe, remove any pinning info */
        unpinConnection(true);
    }

    return pinning.serverConnection;
}