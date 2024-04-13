HttpStateData::closeServer()
{
    debugs(11,5, HERE << "closing HTTP server " << serverConnection << " this " << this);

    if (Comm::IsConnOpen(serverConnection)) {
        fwd->unregister(serverConnection);
        comm_remove_close_handler(serverConnection->fd, closeHandler);
        closeHandler = NULL;
        serverConnection->close();
    }
}