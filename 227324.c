void httpsSslBumpStep2AccessCheckDone(allow_t answer, void *data)
{
    ConnStateData *connState = (ConnStateData *) data;

    // if the connection is closed or closing, just return.
    if (!connState->isOpen())
        return;

    debugs(33, 5, "Answer: " << answer << " kind:" << answer.kind);
    assert(connState->serverBump());
    Ssl::BumpMode bumpAction;
    if (answer.allowed()) {
        bumpAction = (Ssl::BumpMode)answer.kind;
    } else
        bumpAction = Ssl::bumpSplice;

    connState->serverBump()->act.step2 = bumpAction;
    connState->sslBumpMode = bumpAction;
    Http::StreamPointer context = connState->pipeline.front();
    if (ClientHttpRequest *http = (context ? context->http : nullptr))
        http->al->ssl.bumpMode = bumpAction;

    if (bumpAction == Ssl::bumpTerminate) {
        connState->clientConnection->close();
    } else if (bumpAction != Ssl::bumpSplice) {
        connState->startPeekAndSplice();
    } else if (!connState->splice())
        connState->clientConnection->close();
}