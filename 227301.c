ConnStateData::swanSong()
{
    debugs(33, 2, HERE << clientConnection);
    checkLogging();

    flags.readMore = false;
    clientdbEstablished(clientConnection->remote, -1);  /* decrement */
    pipeline.terminateAll(0);

    // XXX: Closing pinned conn is too harsh: The Client may want to continue!
    unpinConnection(true);

    Server::swanSong(); // closes the client connection

#if USE_AUTH
    // NP: do this bit after closing the connections to avoid side effects from unwanted TCP RST
    setAuth(NULL, "ConnStateData::SwanSong cleanup");
#endif

    flags.swanSang = true;
}