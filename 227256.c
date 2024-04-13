ConnStateData::sendControlMsg(HttpControlMsg msg)
{
    if (!isOpen()) {
        debugs(33, 3, HERE << "ignoring 1xx due to earlier closure");
        return;
    }

    // HTTP/1 1xx status messages are only valid when there is a transaction to trigger them
    if (!pipeline.empty()) {
        HttpReply::Pointer rep(msg.reply);
        Must(rep);
        // remember the callback
        cbControlMsgSent = msg.cbSuccess;

        typedef CommCbMemFunT<HttpControlMsgSink, CommIoCbParams> Dialer;
        AsyncCall::Pointer call = JobCallback(33, 5, Dialer, this, HttpControlMsgSink::wroteControlMsg);

        if (!writeControlMsgAndCall(rep.getRaw(), call)) {
            // but still inform the caller (so it may resume its operation)
            doneWithControlMsg();
        }
        return;
    }

    debugs(33, 3, HERE << " closing due to missing context for 1xx");
    clientConnection->close();
}