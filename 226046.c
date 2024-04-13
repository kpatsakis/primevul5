static int session_fixups(request_rec * r)
{
    session_dir_conf *conf = ap_get_module_config(r->per_dir_config,
                                                  &session_module);

    session_rec *z = NULL;

    /* if an error occurs or no session has been configured, we ignore
     * the broken session and allow it to be recreated from scratch on save
     * if necessary.
     */
    ap_session_load(r, &z);

    if (conf->env) {
        if (z) {
            session_identity_encode(r, z);
            if (z->encoded) {
                apr_table_set(r->subprocess_env, HTTP_SESSION, z->encoded);
                z->encoded = NULL;
            }
        }
        apr_table_unset(r->headers_in, "Session");
    }

    return OK;

}