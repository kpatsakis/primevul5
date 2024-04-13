ngx_http_auth_spnego_merge_loc_conf(
        ngx_conf_t * cf,
        void *parent,
        void *child)
{
    ngx_http_auth_spnego_loc_conf_t *prev = parent;
    ngx_http_auth_spnego_loc_conf_t *conf = child;

    /* "off" by default */
    ngx_conf_merge_off_value(conf->protect, prev->protect, 0);

    ngx_conf_merge_str_value(conf->realm, prev->realm, "");
    ngx_conf_merge_str_value(conf->keytab, prev->keytab,
            "/etc/krb5.keytab");
    ngx_conf_merge_str_value(conf->srvcname, prev->srvcname, "");

    ngx_conf_merge_off_value(conf->fqun, prev->fqun, 0);
    ngx_conf_merge_off_value(conf->force_realm, prev->force_realm, 0);
    ngx_conf_merge_off_value(conf->allow_basic, prev->allow_basic, 1);
    ngx_conf_merge_ptr_value(conf->auth_princs, prev->auth_princs, NGX_CONF_UNSET_PTR);

    ngx_conf_merge_off_value(conf->map_to_local, prev->map_to_local, 0);

#if (NGX_DEBUG)
    ngx_conf_log_error(NGX_LOG_INFO, cf, 0, "auth_spnego: protect = %i",
            conf->protect);
    ngx_conf_log_error(NGX_LOG_INFO, cf, 0, "auth_spnego: realm@0x%p = %s",
            conf->realm.data, conf->realm.data);
    ngx_conf_log_error(NGX_LOG_INFO, cf, 0,
            "auth_spnego: keytab@0x%p = %s", conf->keytab.data,
            conf->keytab.data);
    ngx_conf_log_error(NGX_LOG_INFO, cf, 0,
            "auth_spnego: srvcname@0x%p = %s",
            conf->srvcname.data, conf->srvcname.data);
    ngx_conf_log_error(NGX_LOG_INFO, cf, 0, "auth_spnego: fqun = %i",
            conf->fqun);
    ngx_conf_log_error(NGX_LOG_INFO, cf, 0, "auth_spnego: allow_basic = %i",
            conf->allow_basic);
    ngx_conf_log_error(NGX_LOG_INFO, cf, 0, "auth_spnego: force_realm = %i",
            conf->force_realm);
    if (NGX_CONF_UNSET_PTR != conf->auth_princs) {
        size_t ii = 0;
        ngx_str_t *auth_princs = conf->auth_princs->elts;
        for (; ii < conf->auth_princs->nelts; ++ii) {
            ngx_conf_log_error(NGX_LOG_DEBUG, cf, 0,
                    "auth_spnego: auth_princs = %.*s", auth_princs[ii].len, auth_princs[ii].data);
        }
    }
    ngx_conf_log_error(NGX_LOG_INFO, cf, 0, "auth_spnego: map_to_local = %i",
            conf->map_to_local);
#endif

    return NGX_CONF_OK;
}