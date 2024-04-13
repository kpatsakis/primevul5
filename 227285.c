HttpStateData::doneSendingRequestBody()
{
    Client::doneSendingRequestBody();
    debugs(11,5, HERE << serverConnection);

    // do we need to write something after the last body byte?
    if (flags.chunked_request && finishingChunkedRequest())
        return;
    if (!flags.chunked_request && finishingBrokenPost())
        return;

    sendComplete();
}