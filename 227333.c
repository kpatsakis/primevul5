HttpStateData::finishingBrokenPost()
{
#if USE_HTTP_VIOLATIONS
    if (!Config.accessList.brokenPosts) {
        debugs(11, 5, HERE << "No brokenPosts list");
        return false;
    }

    ACLFilledChecklist ch(Config.accessList.brokenPosts, originalRequest(), NULL);
    ch.al = fwd->al;
    ch.syncAle(originalRequest(), nullptr);
    if (!ch.fastCheck().allowed()) {
        debugs(11, 5, HERE << "didn't match brokenPosts");
        return false;
    }

    if (!Comm::IsConnOpen(serverConnection)) {
        debugs(11, 3, HERE << "ignoring broken POST for closed " << serverConnection);
        assert(closeHandler != NULL);
        return true; // prevent caller from proceeding as if nothing happened
    }

    debugs(11, 3, "finishingBrokenPost: fixing broken POST");
    typedef CommCbMemFunT<HttpStateData, CommIoCbParams> Dialer;
    requestSender = JobCallback(11,5,
                                Dialer, this, HttpStateData::wroteLast);
    Comm::Write(serverConnection, "\r\n", 2, requestSender, NULL);
    return true;
#else
    return false;
#endif /* USE_HTTP_VIOLATIONS */
}