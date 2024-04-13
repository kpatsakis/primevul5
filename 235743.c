ngx_http_auth_spnego_headers_basic_only(
    ngx_http_request_t *r,
    ngx_http_auth_spnego_ctx_t *ctx,
    ngx_http_auth_spnego_loc_conf_t *alcf)
{
    ngx_str_t value = ngx_null_string;
    value.len = sizeof("Basic realm=\"\"") - 1 + alcf->realm.len;
    value.data = ngx_pcalloc(r->pool, value.len);
    if (NULL == value.data) {
        return NGX_ERROR;
    }
    ngx_snprintf(value.data, value.len, "Basic realm=\"%V\"",
            &alcf->realm);
    r->headers_out.www_authenticate =
        ngx_list_push(&r->headers_out.headers);
    if (NULL == r->headers_out.www_authenticate) {
        return NGX_ERROR;
    }

    r->headers_out.www_authenticate->hash = 1;
    r->headers_out.www_authenticate->key.len = sizeof("WWW-Authenticate") - 1;
    r->headers_out.www_authenticate->key.data = (u_char *) "WWW-Authenticate";
    r->headers_out.www_authenticate->value.len = value.len;
    r->headers_out.www_authenticate->value.data = value.data;

    ctx->head = 1;

    return NGX_OK;
}