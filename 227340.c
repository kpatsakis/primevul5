httpsEstablish(ConnStateData *connState, const Security::ContextPointer &ctx)
{
    assert(connState);
    const Comm::ConnectionPointer &details = connState->clientConnection;

    if (!ctx || !httpsCreate(connState, ctx))
        return;

    typedef CommCbMemFunT<ConnStateData, CommTimeoutCbParams> TimeoutDialer;
    AsyncCall::Pointer timeoutCall = JobCallback(33, 5, TimeoutDialer,
                                     connState, ConnStateData::requestTimeout);
    commSetConnTimeout(details, Config.Timeout.request, timeoutCall);

    Comm::SetSelect(details->fd, COMM_SELECT_READ, clientNegotiateSSL, connState, 0);
}