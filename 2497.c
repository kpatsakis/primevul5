gopherTimeout(const CommTimeoutCbParams &io)
{
    GopherStateData *gopherState = static_cast<GopherStateData *>(io.data);
    debugs(10, 4, io.conn << ": '" << gopherState->entry->url() << "'" );

    gopherState->fwd->fail(new ErrorState(ERR_READ_TIMEOUT, Http::scGatewayTimeout, gopherState->fwd->request, gopherState->fwd->al));

    if (Comm::IsConnOpen(io.conn))
        io.conn->close();
}