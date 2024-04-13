static apr_status_t ap_session_load(request_rec * r, session_rec ** z)
{

    session_dir_conf *dconf = ap_get_module_config(r->per_dir_config,
                                                   &session_module);
    apr_time_t now;
    session_rec *zz = NULL;
    int rv = 0;

    /* is the session enabled? */
    if (!dconf || !dconf->enabled) {
        return APR_SUCCESS;
    }

    /* should the session be loaded at all? */
    if (!session_included(r, dconf)) {
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01814)
                      "excluded by configuration for: %s", r->uri);
        return APR_SUCCESS;
    }

    /* load the session from the session hook */
    rv = ap_run_session_load(r, &zz);
    if (DECLINED == rv) {
        ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, APLOGNO(01815)
                      "session is enabled but no session modules have been configured, "
                      "session not loaded: %s", r->uri);
        return APR_EGENERAL;
    }
    else if (OK != rv) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(01816)
                      "error while loading the session, "
                      "session not loaded: %s", r->uri);
        return rv;
    }

    /* found a session that hasn't expired? */
    now = apr_time_now();

    if (zz) {
        /* load the session attributes */
        rv = ap_run_session_decode(r, zz);
 
        /* having a session we cannot decode is just as good as having
           none at all */
       if (OK != rv) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(01817)
                    "error while decoding the session, "
                    "session not loaded: %s", r->uri);
            /* preserve pointers to zz in load/save providers */
            memset(zz, 0, sizeof(session_rec));
            zz->pool = r->pool;
            zz->entries = apr_table_make(zz->pool, 10);
        }

       /* invalidate session if session is expired */
        if (zz && zz->expiry && zz->expiry < now) {
            ap_log_rerror(APLOG_MARK, APLOG_TRACE2, 0, r, "session is expired");
            /* preserve pointers to zz in load/save providers */
            memset(zz, 0, sizeof(session_rec));
            zz->pool = r->pool;
            zz->entries = apr_table_make(zz->pool, 10);
        }
    }

    /* no luck, create a blank session. Note that the included session_load 
     * providers will return new sessions during session_load when configured.
     */
    if (!zz) {
        zz = (session_rec *) apr_pcalloc(r->pool, sizeof(session_rec));
        zz->pool = r->pool;
        zz->entries = apr_table_make(zz->pool, 10);
    }

    /* make sure the expiry and maxage are set, if present */
    if (dconf->maxage) {
        if (!zz->expiry) {
            zz->expiry = now + dconf->maxage * APR_USEC_PER_SEC;
        }
        zz->maxage = dconf->maxage;
    }

    *z = zz;

    return APR_SUCCESS;

}