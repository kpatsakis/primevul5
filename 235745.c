ngx_spnego_authorized_principal(
        ngx_http_request_t * r,
        ngx_str_t *princ,
        ngx_http_auth_spnego_loc_conf_t *alcf)
{
    if (NGX_CONF_UNSET_PTR == alcf->auth_princs) {
        return true;
    }
    size_t ii = 0;
    ngx_str_t *auth_princs = alcf->auth_princs->elts;
    spnego_debug1("Testing against %d auth princs", alcf->auth_princs->nelts);
    for (; ii < alcf->auth_princs->nelts; ++ii) {
        if (auth_princs[ii].len != princ->len) {
            continue;
        }
        if (ngx_strncmp(auth_princs[ii].data, princ->data, princ->len) == 0) {
            spnego_debug2("Authorized user %.*s", princ->len, princ->data);
            return true;
        }
    }
    return false;
}