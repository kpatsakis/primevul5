ConnStateData::readNextRequest()
{
    debugs(33, 5, HERE << clientConnection << " reading next req");

    fd_note(clientConnection->fd, "Idle client: Waiting for next request");
    /**
     * Set the timeout BEFORE calling readSomeData().
     */
    typedef CommCbMemFunT<ConnStateData, CommTimeoutCbParams> TimeoutDialer;
    AsyncCall::Pointer timeoutCall = JobCallback(33, 5,
                                     TimeoutDialer, this, ConnStateData::requestTimeout);
    commSetConnTimeout(clientConnection, clientConnection->timeLeft(idleTimeout()), timeoutCall);

    readSomeData();
    /** Please don't do anything with the FD past here! */
}