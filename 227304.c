copyOneHeaderFromClientsideRequestToUpstreamRequest(const HttpHeaderEntry *e, const String strConnection, const HttpRequest * request, HttpHeader * hdr_out, const int we_do_ranges, const Http::StateFlags &flags)
{
    debugs(11, 5, "httpBuildRequestHeader: " << e->name << ": " << e->value );

    switch (e->id) {

    /** \par RFC 2616 sect 13.5.1 - Hop-by-Hop headers which Squid should not pass on. */

    case Http::HdrType::PROXY_AUTHORIZATION:
        /** \par Proxy-Authorization:
         * Only pass on proxy authentication to peers for which
         * authentication forwarding is explicitly enabled
         */
        if (!flags.originpeer && flags.proxying && request->peer_login &&
                (strcmp(request->peer_login, "PASS") == 0 ||
                 strcmp(request->peer_login, "PROXYPASS") == 0 ||
                 strcmp(request->peer_login, "PASSTHRU") == 0)) {
            hdr_out->addEntry(e->clone());
        }
        break;

    /** \par RFC 2616 sect 13.5.1 - Hop-by-Hop headers which Squid does not pass on. */

    case Http::HdrType::CONNECTION:          /** \par Connection: */
    case Http::HdrType::TE:                  /** \par TE: */
    case Http::HdrType::KEEP_ALIVE:          /** \par Keep-Alive: */
    case Http::HdrType::PROXY_AUTHENTICATE:  /** \par Proxy-Authenticate: */
    case Http::HdrType::TRAILER:             /** \par Trailer: */
    case Http::HdrType::UPGRADE:             /** \par Upgrade: */
    case Http::HdrType::TRANSFER_ENCODING:   /** \par Transfer-Encoding: */
        break;

    /** \par OTHER headers I haven't bothered to track down yet. */

    case Http::HdrType::AUTHORIZATION:
        /** \par WWW-Authorization:
         * Pass on WWW authentication */

        if (!flags.originpeer) {
            hdr_out->addEntry(e->clone());
        } else {
            /** \note In accelerators, only forward authentication if enabled
             * (see also httpFixupAuthentication for special cases)
             */
            if (request->peer_login &&
                    (strcmp(request->peer_login, "PASS") == 0 ||
                     strcmp(request->peer_login, "PASSTHRU") == 0 ||
                     strcmp(request->peer_login, "PROXYPASS") == 0)) {
                hdr_out->addEntry(e->clone());
            }
        }

        break;

    case Http::HdrType::HOST:
        /** \par Host:
         * Normally Squid rewrites the Host: header.
         * However, there is one case when we don't: If the URL
         * went through our redirector and the admin configured
         * 'redir_rewrites_host' to be off.
         */
        if (request->peer_domain)
            hdr_out->putStr(Http::HdrType::HOST, request->peer_domain);
        else if (request->flags.redirected && !Config.onoff.redir_rewrites_host)
            hdr_out->addEntry(e->clone());
        else {
            SBuf authority = request->url.authority();
            hdr_out->putStr(Http::HdrType::HOST, authority.c_str());
        }

        break;

    case Http::HdrType::IF_MODIFIED_SINCE:
        /** \par If-Modified-Since:
         * append unless we added our own,
         * but only if cache_miss_revalidate is enabled, or
         *  the request is not cacheable, or
         *  the request contains authentication credentials.
         * \note at most one client's If-Modified-Since header can pass through
         */
        // XXX: need to check and cleanup the auth case so cacheable auth requests get cached.
        if (hdr_out->has(Http::HdrType::IF_MODIFIED_SINCE))
            break;
        else if (Config.onoff.cache_miss_revalidate || !request->flags.cachable || request->flags.auth)
            hdr_out->addEntry(e->clone());
        break;

    case Http::HdrType::IF_NONE_MATCH:
        /** \par If-None-Match:
         * append if the wildcard '*' special case value is present, or
         *   cache_miss_revalidate is disabled, or
         *   the request is not cacheable in this proxy, or
         *   the request contains authentication credentials.
         * \note this header lists a set of responses for the server to elide sending. Squid added values are extending that set.
         */
        // XXX: need to check and cleanup the auth case so cacheable auth requests get cached.
        if (hdr_out->hasListMember(Http::HdrType::IF_MATCH, "*", ',') || Config.onoff.cache_miss_revalidate || !request->flags.cachable || request->flags.auth)
            hdr_out->addEntry(e->clone());
        break;

    case Http::HdrType::MAX_FORWARDS:
        /** \par Max-Forwards:
         * pass only on TRACE or OPTIONS requests */
        if (request->method == Http::METHOD_TRACE || request->method == Http::METHOD_OPTIONS) {
            const int64_t hops = e->getInt64();

            if (hops > 0)
                hdr_out->putInt64(Http::HdrType::MAX_FORWARDS, hops - 1);
        }

        break;

    case Http::HdrType::VIA:
        /** \par Via:
         * If Via is disabled then forward any received header as-is.
         * Otherwise leave for explicit updated addition later. */

        if (!Config.onoff.via)
            hdr_out->addEntry(e->clone());

        break;

    case Http::HdrType::RANGE:

    case Http::HdrType::IF_RANGE:

    case Http::HdrType::REQUEST_RANGE:
        /** \par Range:, If-Range:, Request-Range:
         * Only pass if we accept ranges */
        if (!we_do_ranges)
            hdr_out->addEntry(e->clone());

        break;

    case Http::HdrType::PROXY_CONNECTION: // SHOULD ignore. But doing so breaks things.
        break;

    case Http::HdrType::CONTENT_LENGTH:
        // pass through unless we chunk; also, keeping this away from default
        // prevents request smuggling via Connection: Content-Length tricks
        if (!flags.chunked_request)
            hdr_out->addEntry(e->clone());
        break;

    case Http::HdrType::X_FORWARDED_FOR:

    case Http::HdrType::CACHE_CONTROL:
        /** \par X-Forwarded-For:, Cache-Control:
         * handled specially by Squid, so leave off for now.
         * append these after the loop if needed */
        break;

    case Http::HdrType::FRONT_END_HTTPS:
        /** \par Front-End-Https:
         * Pass thru only if peer is configured with front-end-https */
        if (!flags.front_end_https)
            hdr_out->addEntry(e->clone());

        break;

    default:
        /** \par default.
         * pass on all other header fields
         * which are NOT listed by the special Connection: header. */

        if (strConnection.size()>0 && strListIsMember(&strConnection, e->name.termedBuf(), ',')) {
            debugs(11, 2, "'" << e->name << "' header cropped by Connection: definition");
            return;
        }

        hdr_out->addEntry(e->clone());
    }
}