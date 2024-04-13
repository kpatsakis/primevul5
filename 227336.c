HttpStateData::haveParsedReplyHeaders()
{
    Client::haveParsedReplyHeaders();

    Ctx ctx = ctx_enter(entry->mem_obj->urlXXX());
    HttpReply *rep = finalReply();
    const Http::StatusCode statusCode = rep->sline.status();

    entry->timestampsSet();

    /* Check if object is cacheable or not based on reply code */
    debugs(11, 3, "HTTP CODE: " << statusCode);

    if (StoreEntry *oldEntry = findPreviouslyCachedEntry(entry)) {
        oldEntry->lock("HttpStateData::haveParsedReplyHeaders");
        sawDateGoBack = rep->olderThan(oldEntry->getReply());
        oldEntry->unlock("HttpStateData::haveParsedReplyHeaders");
    }

    if (neighbors_do_private_keys && !sawDateGoBack)
        httpMaybeRemovePublic(entry, rep->sline.status());

    bool varyFailure = false;
    if (rep->header.has(Http::HdrType::VARY)
#if X_ACCELERATOR_VARY
            || rep->header.has(Http::HdrType::HDR_X_ACCELERATOR_VARY)
#endif
       ) {
        const SBuf vary(httpMakeVaryMark(request, rep));

        if (vary.isEmpty()) {
            // TODO: check whether such responses are shareable.
            // Do not share for now.
            entry->makePrivate(false);
            if (fwd->reforwardableStatus(rep->sline.status()))
                EBIT_SET(entry->flags, ENTRY_FWD_HDR_WAIT);
            varyFailure = true;
        } else {
            entry->mem_obj->vary_headers = vary;

            // RFC 7231 section 7.1.4
            // Vary:* can be cached, but has mandatory revalidation
            static const SBuf asterisk("*");
            if (vary == asterisk)
                EBIT_SET(entry->flags, ENTRY_REVALIDATE_ALWAYS);
        }
    }

    if (!varyFailure) {
        /*
         * If its not a reply that we will re-forward, then
         * allow the client to get it.
         */
        if (fwd->reforwardableStatus(rep->sline.status()))
            EBIT_SET(entry->flags, ENTRY_FWD_HDR_WAIT);

        ReuseDecision decision(entry, statusCode);

        switch (reusableReply(decision)) {

        case ReuseDecision::reuseNot:
            entry->makePrivate(false);
            break;

        case ReuseDecision::cachePositively:
            if (!entry->makePublic()) {
                decision.make(ReuseDecision::doNotCacheButShare, "public key creation error");
                entry->makePrivate(true);
            }
            break;

        case ReuseDecision::cacheNegatively:
            if (!entry->cacheNegatively()) {
                decision.make(ReuseDecision::doNotCacheButShare, "public key creation error");
                entry->makePrivate(true);
            }
            break;

        case ReuseDecision::doNotCacheButShare:
            entry->makePrivate(true);
            break;

        default:
            assert(0);
            break;
        }
        debugs(11, 3, "decided: " << decision);
    }

    if (!ignoreCacheControl) {
        if (rep->cache_control) {
            // We are required to revalidate on many conditions.
            // For security reasons we do so even if storage was caused by refresh_pattern ignore-* option

            // CC:must-revalidate or CC:proxy-revalidate
            const bool ccMustRevalidate = (rep->cache_control->hasProxyRevalidate() || rep->cache_control->hasMustRevalidate());

            // CC:no-cache (only if there are no parameters)
            const bool ccNoCacheNoParams = rep->cache_control->hasNoCacheWithoutParameters();

            // CC:s-maxage=N
            const bool ccSMaxAge = rep->cache_control->hasSMaxAge();

            // CC:private (yes, these can sometimes be stored)
            const bool ccPrivate = rep->cache_control->hasPrivate();

            if (ccNoCacheNoParams || ccPrivate)
                EBIT_SET(entry->flags, ENTRY_REVALIDATE_ALWAYS);
            else if (ccMustRevalidate || ccSMaxAge)
                EBIT_SET(entry->flags, ENTRY_REVALIDATE_STALE);
        }
#if USE_HTTP_VIOLATIONS // response header Pragma::no-cache is undefined in HTTP
        else {
            // Expensive calculation. So only do it IF the CC: header is not present.

            /* HACK: Pragma: no-cache in _replies_ is not documented in HTTP,
             * but servers like "Active Imaging Webcast/2.0" sure do use it */
            if (rep->header.has(Http::HdrType::PRAGMA) &&
                    rep->header.hasListMember(Http::HdrType::PRAGMA,"no-cache",','))
                EBIT_SET(entry->flags, ENTRY_REVALIDATE_ALWAYS);
        }
#endif
    }

#if HEADERS_LOG
    headersLog(1, 0, request->method, rep);

#endif

    ctx_exit(ctx);
}