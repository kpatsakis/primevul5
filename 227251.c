httpMaybeRemovePublic(StoreEntry * e, Http::StatusCode status)
{
    int remove = 0;
    int forbidden = 0;

    // If the incoming response already goes into a public entry, then there is
    // nothing to remove. This protects ready-for-collapsing entries as well.
    if (!EBIT_TEST(e->flags, KEY_PRIVATE))
        return;

    // If the new/incoming response cannot be stored, then it does not
    // compete with the old stored response for the public key, and the
    // old stored response should be left as is.
    if (e->mem_obj->request && !e->mem_obj->request->flags.cachable)
        return;

    switch (status) {

    case Http::scOkay:

    case Http::scNonAuthoritativeInformation:

    case Http::scMultipleChoices:

    case Http::scMovedPermanently:

    case Http::scFound:

    case Http::scSeeOther:

    case Http::scGone:

    case Http::scNotFound:
        remove = 1;

        break;

    case Http::scForbidden:

    case Http::scMethodNotAllowed:
        forbidden = 1;

        break;

#if WORK_IN_PROGRESS

    case Http::scUnauthorized:
        forbidden = 1;

        break;

#endif

    default:
#if QUESTIONABLE
        /*
         * Any 2xx response should eject previously cached entities...
         */

        if (status >= 200 && status < 300)
            remove = 1;

#endif

        break;
    }

    if (!remove && !forbidden)
        return;

    StoreEntry *pe = findPreviouslyCachedEntry(e);

    if (pe != NULL) {
        assert(e != pe);
#if USE_HTCP
        neighborsHtcpClear(e, e->mem_obj->request, e->mem_obj->method, HTCP_CLR_INVALIDATION);
#endif
        pe->release(true);
    }

    /** \par
     * Also remove any cached HEAD response in case the object has
     * changed.
     */
    if (e->mem_obj->request)
        pe = storeGetPublicByRequestMethod(e->mem_obj->request, Http::METHOD_HEAD);
    else
        pe = storeGetPublic(e->mem_obj->storeId(), Http::METHOD_HEAD);

    if (pe != NULL) {
        assert(e != pe);
#if USE_HTCP
        neighborsHtcpClear(e, e->mem_obj->request, HttpRequestMethod(Http::METHOD_HEAD), HTCP_CLR_INVALIDATION);
#endif
        pe->release(true);
    }
}