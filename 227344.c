ConnStateData::requestTimeout(const CommTimeoutCbParams &io)
{
    if (!Comm::IsConnOpen(io.conn))
        return;

    const err_type error = receivedFirstByte_ ? ERR_REQUEST_PARSE_TIMEOUT : ERR_REQUEST_START_TIMEOUT;
    if (tunnelOnError(HttpRequestMethod(), error))
        return;

    /*
    * Just close the connection to not confuse browsers
    * using persistent connections. Some browsers open
    * a connection and then do not use it until much
    * later (presumeably because the request triggering
    * the open has already been completed on another
    * connection)
    */
    debugs(33, 3, "requestTimeout: FD " << io.fd << ": lifetime is expired.");
    io.conn->close();
}