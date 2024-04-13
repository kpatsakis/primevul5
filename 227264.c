HttpStateData::httpBuildRequestHeader(HttpRequest * request,
                                      StoreEntry * entry,
                                      const AccessLogEntryPointer &al,
                                      HttpHeader * hdr_out,
                                      const Http::StateFlags &flags)
{
    /* building buffer for complex strings */
#define BBUF_SZ (MAX_URL+32)
    LOCAL_ARRAY(char, bbuf, BBUF_SZ);
    LOCAL_ARRAY(char, ntoabuf, MAX_IPSTRLEN);
    const HttpHeader *hdr_in = &request->header;
    const HttpHeaderEntry *e = NULL;
    HttpHeaderPos pos = HttpHeaderInitPos;
    assert (hdr_out->owner == hoRequest);

    /* use our IMS header if the cached entry has Last-Modified time */
    if (request->lastmod > -1)
        hdr_out->putTime(Http::HdrType::IF_MODIFIED_SINCE, request->lastmod);

    // Add our own If-None-Match field if the cached entry has a strong ETag.
    // copyOneHeaderFromClientsideRequestToUpstreamRequest() adds client ones.
    if (request->etag.size() > 0) {
        hdr_out->addEntry(new HttpHeaderEntry(Http::HdrType::IF_NONE_MATCH, NULL,
                                              request->etag.termedBuf()));
    }

    bool we_do_ranges = decideIfWeDoRanges (request);

    String strConnection (hdr_in->getList(Http::HdrType::CONNECTION));

    while ((e = hdr_in->getEntry(&pos)))
        copyOneHeaderFromClientsideRequestToUpstreamRequest(e, strConnection, request, hdr_out, we_do_ranges, flags);

    /* Abstraction break: We should interpret multipart/byterange responses
     * into offset-length data, and this works around our inability to do so.
     */
    if (!we_do_ranges && request->multipartRangeRequest()) {
        /* don't cache the result */
        request->flags.cachable = false;
        /* pretend it's not a range request */
        request->ignoreRange("want to request the whole object");
        request->flags.isRanged = false;
    }

    hdr_out->addVia(request->http_ver, hdr_in);

    if (request->flags.accelerated) {
        /* Append Surrogate-Capabilities */
        String strSurrogate(hdr_in->getList(Http::HdrType::SURROGATE_CAPABILITY));
#if USE_SQUID_ESI
        snprintf(bbuf, BBUF_SZ, "%s=\"Surrogate/1.0 ESI/1.0\"", Config.Accel.surrogate_id);
#else
        snprintf(bbuf, BBUF_SZ, "%s=\"Surrogate/1.0\"", Config.Accel.surrogate_id);
#endif
        strListAdd(&strSurrogate, bbuf, ',');
        hdr_out->putStr(Http::HdrType::SURROGATE_CAPABILITY, strSurrogate.termedBuf());
    }

    /** \pre Handle X-Forwarded-For */
    if (strcmp(opt_forwarded_for, "delete") != 0) {

        String strFwd = hdr_in->getList(Http::HdrType::X_FORWARDED_FOR);

        // if we cannot double strFwd size, then it grew past 50% of the limit
        if (!strFwd.canGrowBy(strFwd.size())) {
            // There is probably a forwarding loop with Via detection disabled.
            // If we do nothing, String will assert on overflow soon.
            // TODO: Terminate all transactions with huge XFF?
            strFwd = "error";

            static int warnedCount = 0;
            if (warnedCount++ < 100) {
                const SBuf url(entry ? SBuf(entry->url()) : request->effectiveRequestUri());
                debugs(11, DBG_IMPORTANT, "Warning: likely forwarding loop with " << url);
            }
        }

        if (strcmp(opt_forwarded_for, "on") == 0) {
            /** If set to ON - append client IP or 'unknown'. */
            if ( request->client_addr.isNoAddr() )
                strListAdd(&strFwd, "unknown", ',');
            else
                strListAdd(&strFwd, request->client_addr.toStr(ntoabuf, MAX_IPSTRLEN), ',');
        } else if (strcmp(opt_forwarded_for, "off") == 0) {
            /** If set to OFF - append 'unknown'. */
            strListAdd(&strFwd, "unknown", ',');
        } else if (strcmp(opt_forwarded_for, "transparent") == 0) {
            /** If set to TRANSPARENT - pass through unchanged. */
        } else if (strcmp(opt_forwarded_for, "truncate") == 0) {
            /** If set to TRUNCATE - drop existing list and replace with client IP or 'unknown'. */
            if ( request->client_addr.isNoAddr() )
                strFwd = "unknown";
            else
                strFwd = request->client_addr.toStr(ntoabuf, MAX_IPSTRLEN);
        }
        if (strFwd.size() > 0)
            hdr_out->putStr(Http::HdrType::X_FORWARDED_FOR, strFwd.termedBuf());
    }
    /** If set to DELETE - do not copy through. */

    /* append Host if not there already */
    if (!hdr_out->has(Http::HdrType::HOST)) {
        if (request->peer_domain) {
            hdr_out->putStr(Http::HdrType::HOST, request->peer_domain);
        } else {
            SBuf authority = request->url.authority();
            hdr_out->putStr(Http::HdrType::HOST, authority.c_str());
        }
    }

    /* append Authorization if known in URL, not in header and going direct */
    if (!hdr_out->has(Http::HdrType::AUTHORIZATION)) {
        if (!request->flags.proxying && !request->url.userInfo().isEmpty()) {
            static char result[base64_encode_len(MAX_URL*2)]; // should be big enough for a single URI segment
            struct base64_encode_ctx ctx;
            base64_encode_init(&ctx);
            size_t blen = base64_encode_update(&ctx, result, request->url.userInfo().length(), reinterpret_cast<const uint8_t*>(request->url.userInfo().rawContent()));
            blen += base64_encode_final(&ctx, result+blen);
            result[blen] = '\0';
            if (blen)
                httpHeaderPutStrf(hdr_out, Http::HdrType::AUTHORIZATION, "Basic %.*s", (int)blen, result);
        }
    }

    /* Fixup (Proxy-)Authorization special cases. Plain relaying dealt with above */
    httpFixupAuthentication(request, hdr_in, hdr_out, flags);

    /* append Cache-Control, add max-age if not there already */
    {
        HttpHdrCc *cc = hdr_in->getCc();

        if (!cc)
            cc = new HttpHdrCc();

#if 0 /* see bug 2330 */
        /* Set no-cache if determined needed but not found */
        if (request->flags.nocache)
            EBIT_SET(cc->mask, HttpHdrCcType::CC_NO_CACHE);
#endif

        /* Add max-age only without no-cache */
        if (!cc->hasMaxAge() && !cc->hasNoCache()) {
            // XXX: performance regression. c_str() reallocates
            SBuf tmp(request->effectiveRequestUri());
            cc->maxAge(getMaxAge(entry ? entry->url() : tmp.c_str()));
        }

        /* Enforce sibling relations */
        if (flags.only_if_cached)
            cc->onlyIfCached(true);

        hdr_out->putCc(cc);

        delete cc;
    }

    // Always send Connection because HTTP/1.0 servers need explicit "keep-alive"
    // while HTTP/1.1 servers need explicit "close", and we do not always know
    // the server expectations.
    hdr_out->putStr(Http::HdrType::CONNECTION, flags.keepalive ? "keep-alive" : "close");

    /* append Front-End-Https */
    if (flags.front_end_https) {
        if (flags.front_end_https == 1 || request->url.getScheme() == AnyP::PROTO_HTTPS)
            hdr_out->putStr(Http::HdrType::FRONT_END_HTTPS, "On");
    }

    if (flags.chunked_request) {
        // Do not just copy the original value so that if the client-side
        // starts decode other encodings, this code may remain valid.
        hdr_out->putStr(Http::HdrType::TRANSFER_ENCODING, "chunked");
    }

    /* Now mangle the headers. */
    httpHdrMangleList(hdr_out, request, al, ROR_REQUEST);

    strConnection.clean();
}