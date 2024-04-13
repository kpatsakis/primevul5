HttpStateData::finishingChunkedRequest()
{
    if (flags.sentLastChunk) {
        debugs(11, 5, HERE << "already sent last-chunk");
        return false;
    }

    Must(receivedWholeRequestBody); // or we should not be sending last-chunk
    flags.sentLastChunk = true;

    typedef CommCbMemFunT<HttpStateData, CommIoCbParams> Dialer;
    requestSender = JobCallback(11,5, Dialer, this, HttpStateData::wroteLast);
    Comm::Write(serverConnection, "0\r\n\r\n", 5, requestSender, NULL);
    return true;
}