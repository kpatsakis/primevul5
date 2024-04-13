HttpStateData::start()
{
    if (!sendRequest()) {
        debugs(11, 3, "httpStart: aborted");
        mustStop("HttpStateData::start failed");
        return;
    }

    ++ statCounter.server.all.requests;
    ++ statCounter.server.http.requests;

    /*
     * We used to set the read timeout here, but not any more.
     * Now its set in httpSendComplete() after the full request,
     * including request body, has been written to the server.
     */
}