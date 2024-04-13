HttpStateData::reusableReply(HttpStateData::ReuseDecision &decision)
{
    HttpReply const *rep = finalReply();
    HttpHeader const *hdr = &rep->header;
    const char *v;
#if USE_HTTP_VIOLATIONS

    const RefreshPattern *R = NULL;

    /* This strange looking define first looks up the refresh pattern
     * and then checks if the specified flag is set. The main purpose
     * of this is to simplify the refresh pattern lookup and USE_HTTP_VIOLATIONS
     * condition
     */
#define REFRESH_OVERRIDE(flag) \
    ((R = (R ? R : refreshLimits(entry->mem_obj->storeId()))) , \
    (R && R->flags.flag))
#else
#define REFRESH_OVERRIDE(flag) 0
#endif

    if (EBIT_TEST(entry->flags, RELEASE_REQUEST))
        return decision.make(ReuseDecision::doNotCacheButShare, "the entry has been released");

    // RFC 7234 section 4: a cache MUST use the most recent response
    // (as determined by the Date header field)
    // TODO: whether such responses could be shareable?
    if (sawDateGoBack)
        return decision.make(ReuseDecision::reuseNot, "the response has an older date header");

    // Check for Surrogate/1.0 protocol conditions
    // NP: reverse-proxy traffic our parent server has instructed us never to cache
    if (surrogateNoStore)
        return decision.make(ReuseDecision::reuseNot, "Surrogate-Control:no-store");

    // RFC 2616: HTTP/1.1 Cache-Control conditions
    if (!ignoreCacheControl) {
        // XXX: check to see if the request headers alone were enough to prevent caching earlier
        // (ie no-store request header) no need to check those all again here if so.
        // for now we are not reliably doing that so we waste CPU re-checking request CC

        // RFC 2616 section 14.9.2 - MUST NOT cache any response with request CC:no-store
        if (request && request->cache_control && request->cache_control->hasNoStore() &&
                !REFRESH_OVERRIDE(ignore_no_store))
            return decision.make(ReuseDecision::reuseNot,
                                 "client request Cache-Control:no-store");

        // NP: request CC:no-cache only means cache READ is forbidden. STORE is permitted.
        if (rep->cache_control && rep->cache_control->hasNoCacheWithParameters()) {
            /* TODO: we are allowed to cache when no-cache= has parameters.
             * Provided we strip away any of the listed headers unless they are revalidated
             * successfully (ie, must revalidate AND these headers are prohibited on stale replies).
             * That is a bit tricky for squid right now so we avoid caching entirely.
             */
            return decision.make(ReuseDecision::reuseNot,
                                 "server reply Cache-Control:no-cache has parameters");
        }

        // NP: request CC:private is undefined. We ignore.
        // NP: other request CC flags are limiters on HIT/MISS. We don't care about here.

        // RFC 2616 section 14.9.2 - MUST NOT cache any response with CC:no-store
        if (rep->cache_control && rep->cache_control->hasNoStore() &&
                !REFRESH_OVERRIDE(ignore_no_store))
            return decision.make(ReuseDecision::reuseNot,
                                 "server reply Cache-Control:no-store");

        // RFC 2616 section 14.9.1 - MUST NOT cache any response with CC:private in a shared cache like Squid.
        // CC:private overrides CC:public when both are present in a response.
        // TODO: add a shared/private cache configuration possibility.
        if (rep->cache_control &&
                rep->cache_control->hasPrivate() &&
                !REFRESH_OVERRIDE(ignore_private)) {
            /* TODO: we are allowed to cache when private= has parameters.
             * Provided we strip away any of the listed headers unless they are revalidated
             * successfully (ie, must revalidate AND these headers are prohibited on stale replies).
             * That is a bit tricky for squid right now so we avoid caching entirely.
             */
            return decision.make(ReuseDecision::reuseNot,
                                 "server reply Cache-Control:private");
        }
    }

    // RFC 2068, sec 14.9.4 - MUST NOT cache any response with Authentication UNLESS certain CC controls are present
    // allow HTTP violations to IGNORE those controls (ie re-block caching Auth)
    if (request && (request->flags.auth || request->flags.authSent)) {
        if (!rep->cache_control)
            return decision.make(ReuseDecision::reuseNot,
                                 "authenticated and server reply missing Cache-Control");

        if (ignoreCacheControl)
            return decision.make(ReuseDecision::reuseNot,
                                 "authenticated and ignoring Cache-Control");

        bool mayStore = false;
        // HTTPbis pt6 section 3.2: a response CC:public is present
        if (rep->cache_control->hasPublic()) {
            debugs(22, 3, HERE << "Authenticated but server reply Cache-Control:public");
            mayStore = true;

            // HTTPbis pt6 section 3.2: a response CC:must-revalidate is present
        } else if (rep->cache_control->hasMustRevalidate()) {
            debugs(22, 3, HERE << "Authenticated but server reply Cache-Control:must-revalidate");
            mayStore = true;

#if USE_HTTP_VIOLATIONS
            // NP: given the must-revalidate exception we should also be able to exempt no-cache.
            // HTTPbis WG verdict on this is that it is omitted from the spec due to being 'unexpected' by
            // some. The caching+revalidate is not exactly unsafe though with Squids interpretation of no-cache
            // (without parameters) as equivalent to must-revalidate in the reply.
        } else if (rep->cache_control->hasNoCacheWithoutParameters()) {
            debugs(22, 3, HERE << "Authenticated but server reply Cache-Control:no-cache (equivalent to must-revalidate)");
            mayStore = true;
#endif

            // HTTPbis pt6 section 3.2: a response CC:s-maxage is present
        } else if (rep->cache_control->hasSMaxAge()) {
            debugs(22, 3, HERE << "Authenticated but server reply Cache-Control:s-maxage");
            mayStore = true;
        }

        if (!mayStore)
            return decision.make(ReuseDecision::reuseNot, "authenticated transaction");

        // NP: response CC:no-cache is equivalent to CC:must-revalidate,max-age=0. We MAY cache, and do so.
        // NP: other request CC flags are limiters on HIT/MISS/REFRESH. We don't care about here.
    }

    /* HACK: The "multipart/x-mixed-replace" content type is used for
     * continuous push replies.  These are generally dynamic and
     * probably should not be cachable
     */
    if ((v = hdr->getStr(Http::HdrType::CONTENT_TYPE)))
        if (!strncasecmp(v, "multipart/x-mixed-replace", 25))
            return decision.make(ReuseDecision::reuseNot, "Content-Type:multipart/x-mixed-replace");

    // TODO: if possible, provide more specific message for each status code
    static const char *shareableError = "shareable error status code";
    static const char *nonShareableError = "non-shareable error status code";
    ReuseDecision::Answers statusAnswer = ReuseDecision::reuseNot;
    const char *statusReason = nonShareableError;

    switch (rep->sline.status()) {

    /* There are several situations when a non-cacheable response may be
     * still shareable (e.g., among collapsed clients). We assume that these
     * are 3xx and 5xx responses, indicating server problems and some of
     * 4xx responses, common for all clients with a given cache key (e.g.,
     * 404 Not Found or 414 URI Too Long). On the other hand, we should not
     * share non-cacheable client-specific errors, such as 400 Bad Request
     * or 406 Not Acceptable.
     */

    /* Responses that are cacheable */

    case Http::scOkay:

    case Http::scNonAuthoritativeInformation:

    case Http::scMultipleChoices:

    case Http::scMovedPermanently:
    case Http::scPermanentRedirect:

    case Http::scGone:
        /*
         * Don't cache objects that need to be refreshed on next request,
         * unless we know how to refresh it.
         */

        if (refreshIsCachable(entry) || REFRESH_OVERRIDE(store_stale))
            decision.make(ReuseDecision::cachePositively, "refresh check returned cacheable");
        else
            decision.make(ReuseDecision::doNotCacheButShare, "refresh check returned non-cacheable");
        break;

    /* Responses that only are cacheable if the server says so */

    case Http::scFound:
    case Http::scTemporaryRedirect:
        if (rep->date <= 0)
            decision.make(ReuseDecision::doNotCacheButShare, "Date is missing/invalid");
        else if (rep->expires > rep->date)
            decision.make(ReuseDecision::cachePositively, "Expires > Date");
        else
            decision.make(ReuseDecision::doNotCacheButShare, "Expires <= Date");
        break;

    /* These responses can be negatively cached. Most can also be shared. */
    case Http::scNoContent:
    case Http::scUseProxy:
    case Http::scForbidden:
    case Http::scNotFound:
    case Http::scMethodNotAllowed:
    case Http::scUriTooLong:
    case Http::scInternalServerError:
    case Http::scNotImplemented:
    case Http::scBadGateway:
    case Http::scServiceUnavailable:
    case Http::scGatewayTimeout:
    case Http::scMisdirectedRequest:
        statusAnswer = ReuseDecision::doNotCacheButShare;
        statusReason = shareableError;
    // fall through to the actual decision making below

    case Http::scBadRequest: // no sharing; perhaps the server did not like something specific to this request
#if USE_HTTP_VIOLATIONS
        if (Config.negativeTtl > 0)
            decision.make(ReuseDecision::cacheNegatively, "Config.negativeTtl > 0");
        else
#endif
            decision.make(statusAnswer, statusReason);
        break;

    /* these responses can never be cached, some
       of them can be shared though */
    case Http::scSeeOther:
    case Http::scNotModified:
    case Http::scUnauthorized:
    case Http::scProxyAuthenticationRequired:
    case Http::scPaymentRequired:
    case Http::scInsufficientStorage:
        // TODO: use more specific reason for non-error status codes
        decision.make(ReuseDecision::doNotCacheButShare, shareableError);
        break;

    case Http::scPartialContent: /* Not yet supported. TODO: make shareable for suitable ranges */
    case Http::scNotAcceptable:
    case Http::scRequestTimeout: // TODO: is this shareable?
    case Http::scConflict: // TODO: is this shareable?
    case Http::scLengthRequired:
    case Http::scPreconditionFailed:
    case Http::scPayloadTooLarge:
    case Http::scUnsupportedMediaType:
    case Http::scUnprocessableEntity:
    case Http::scLocked: // TODO: is this shareable?
    case Http::scFailedDependency:
    case Http::scRequestedRangeNotSatisfied:
    case Http::scExpectationFailed:
    case Http::scInvalidHeader: /* Squid header parsing error */
    case Http::scHeaderTooLarge:
        decision.make(ReuseDecision::reuseNot, nonShareableError);
        break;

    default:
        /* RFC 2616 section 6.1.1: an unrecognized response MUST NOT be cached. */
        decision.make(ReuseDecision::reuseNot, "unknown status code");
        break;
    }

    return decision.answer;
}