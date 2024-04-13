httpsSslBumpAccessCheckDone(allow_t answer, void *data)
{
    ConnStateData *connState = (ConnStateData *) data;

    // if the connection is closed or closing, just return.
    if (!connState->isOpen())
        return;

    if (answer.allowed()) {
        debugs(33, 2, "sslBump action " << Ssl::bumpMode(answer.kind) << "needed for " << connState->clientConnection);
        connState->sslBumpMode = static_cast<Ssl::BumpMode>(answer.kind);
    } else {
        debugs(33, 3, "sslBump not needed for " << connState->clientConnection);
        connState->sslBumpMode = Ssl::bumpSplice;
    }

    if (connState->sslBumpMode == Ssl::bumpTerminate) {
        connState->clientConnection->close();
        return;
    }

    if (!connState->fakeAConnectRequest("ssl-bump", connState->inBuf))
        connState->clientConnection->close();
}