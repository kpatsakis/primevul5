ConnStateData::borrowPinnedConnection(HttpRequest *request, const CachePeer *aPeer)
{
    debugs(33, 7, pinning.serverConnection);
    if (validatePinnedConnection(request, aPeer) != NULL)
        stopPinnedConnectionMonitoring();

    return pinning.serverConnection; // closed if validation failed
}