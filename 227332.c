ConnStateData::checkLogging()
{
    // if we are parsing request body, its request is responsible for logging
    if (bodyPipe)
        return;

    // a request currently using this connection is responsible for logging
    if (!pipeline.empty() && pipeline.back()->mayUseConnection())
        return;

    /* Either we are waiting for the very first transaction, or
     * we are done with the Nth transaction and are waiting for N+1st.
     * XXX: We assume that if anything was added to inBuf, then it could
     * only be consumed by actions already covered by the above checks.
     */

    // do not log connections that closed after a transaction (it is normal)
    // TODO: access_log needs ACLs to match received-no-bytes connections
    if (pipeline.nrequests && inBuf.isEmpty())
        return;

    /* Create a temporary ClientHttpRequest object. Its destructor will log. */
    ClientHttpRequest http(this);
    http.req_sz = inBuf.length();
    // XXX: Or we died while waiting for the pinned connection to become idle.
    http.setErrorUri("error:transaction-end-before-headers");
}