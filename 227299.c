clientLifetimeTimeout(const CommTimeoutCbParams &io)
{
    ClientHttpRequest *http = static_cast<ClientHttpRequest *>(io.data);
    debugs(33, DBG_IMPORTANT, "WARNING: Closing client connection due to lifetime timeout");
    debugs(33, DBG_IMPORTANT, "\t" << http->uri);
    http->logType.err.timedout = true;
    if (Comm::IsConnOpen(io.conn))
        io.conn->close();
}