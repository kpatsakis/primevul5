HttpStateData::httpTimeout(const CommTimeoutCbParams &)
{
    debugs(11, 4, serverConnection << ": '" << entry->url() << "'");

    if (entry->store_status == STORE_PENDING) {
        fwd->fail(new ErrorState(ERR_READ_TIMEOUT, Http::scGatewayTimeout, fwd->request));
    }

    closeServer();
    mustStop("HttpStateData::httpTimeout");
}