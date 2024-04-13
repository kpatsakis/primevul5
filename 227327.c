HttpStateData::maybeReadVirginBody()
{
    // too late to read
    if (!Comm::IsConnOpen(serverConnection) || fd_table[serverConnection->fd].closing())
        return;

    if (!maybeMakeSpaceAvailable(false))
        return;

    // XXX: get rid of the do_next_read flag
    // check for the proper reasons preventing read(2)
    if (!flags.do_next_read)
        return;

    flags.do_next_read = false;

    // must not already be waiting for read(2) ...
    assert(!Comm::MonitorsRead(serverConnection->fd));

    // wait for read(2) to be possible.
    typedef CommCbMemFunT<HttpStateData, CommIoCbParams> Dialer;
    AsyncCall::Pointer call = JobCallback(11, 5, Dialer, this, HttpStateData::readReply);
    Comm::Read(serverConnection, call);
}