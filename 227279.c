HttpStateData::handle1xx(HttpReply *reply)
{
    HttpReply::Pointer msg(reply); // will destroy reply if unused

    // one 1xx at a time: we must not be called while waiting for previous 1xx
    Must(!flags.handling1xx);
    flags.handling1xx = true;

    if (!request->canHandle1xx() || request->forcedBodyContinuation) {
        debugs(11, 2, "ignoring 1xx because it is " << (request->forcedBodyContinuation ? "already sent" : "not supported by client"));
        proceedAfter1xx();
        return;
    }

#if USE_HTTP_VIOLATIONS
    // check whether the 1xx response forwarding is allowed by squid.conf
    if (Config.accessList.reply) {
        ACLFilledChecklist ch(Config.accessList.reply, originalRequest(), NULL);
        ch.al = fwd->al;
        ch.reply = reply;
        ch.syncAle(originalRequest(), nullptr);
        HTTPMSGLOCK(ch.reply);
        if (!ch.fastCheck().allowed()) { // TODO: support slow lookups?
            debugs(11, 3, HERE << "ignoring denied 1xx");
            proceedAfter1xx();
            return;
        }
    }
#endif // USE_HTTP_VIOLATIONS

    debugs(11, 2, HERE << "forwarding 1xx to client");

    // the Sink will use this to call us back after writing 1xx to the client
    typedef NullaryMemFunT<HttpStateData> CbDialer;
    const AsyncCall::Pointer cb = JobCallback(11, 3, CbDialer, this,
                                  HttpStateData::proceedAfter1xx);
    CallJobHere1(11, 4, request->clientConnectionManager, ConnStateData,
                 ConnStateData::sendControlMsg, HttpControlMsg(msg, cb));
    // If the call is not fired, then the Sink is gone, and HttpStateData
    // will terminate due to an aborted store entry or another similar error.
    // If we get stuck, it is not handle1xx fault if we could get stuck
    // for similar reasons without a 1xx response.
}