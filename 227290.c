HttpStateData::handleRequestBodyProducerAborted()
{
    Client::handleRequestBodyProducerAborted();
    if (entry->isEmpty()) {
        debugs(11, 3, "request body aborted: " << serverConnection);
        // We usually get here when ICAP REQMOD aborts during body processing.
        // We might also get here if client-side aborts, but then our response
        // should not matter because either client-side will provide its own or
        // there will be no response at all (e.g., if the the client has left).
        ErrorState *err = new ErrorState(ERR_ICAP_FAILURE, Http::scInternalServerError, fwd->request);
        err->detailError(ERR_DETAIL_SRV_REQMOD_REQ_BODY);
        fwd->fail(err);
    }

    abortTransaction("request body producer aborted");
}