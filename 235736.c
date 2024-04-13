ngx_http_auth_spnego_headers(
        ngx_http_request_t *r,
        ngx_http_auth_spnego_ctx_t *ctx,
        ngx_str_t *token,
        ngx_http_auth_spnego_loc_conf_t *alcf)
{
    ngx_str_t value = ngx_null_string;
    /* only use token if authorized as there appears to be a bug in
     * Google Chrome when parsing a 401 Negotiate with a token */
    if (NULL == token || ctx->ret != NGX_OK) {
        value.len = sizeof("Negotiate") - 1;
        value.data = (u_char *) "Negotiate";
    } else {
        value.len = sizeof("Negotiate") + token->len; /* space accounts for \0 */
        value.data = ngx_pcalloc(r->pool, value.len);
        if (NULL == value.data) {
            return NGX_ERROR;
        }
        ngx_snprintf(value.data, value.len, "Negotiate %V", token);
    }

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

    if (alcf->allow_basic) {
        ngx_str_t value2 = ngx_null_string;
        value2.len = sizeof("Basic realm=\"\"") - 1 + alcf->realm.len;
        value2.data = ngx_pcalloc(r->pool, value2.len);
        if (NULL == value2.data) {
            return NGX_ERROR;
        }
        ngx_snprintf(value2.data, value2.len, "Basic realm=\"%V\"",
                &alcf->realm);
        r->headers_out.www_authenticate =
            ngx_list_push(&r->headers_out.headers);
        if (NULL == r->headers_out.www_authenticate) {
            return NGX_ERROR;
        }

        r->headers_out.www_authenticate->hash = 2;
        r->headers_out.www_authenticate->key.len = sizeof("WWW-Authenticate") - 1;
        r->headers_out.www_authenticate->key.data = (u_char *) "WWW-Authenticate";
        r->headers_out.www_authenticate->value.len = value2.len;
        r->headers_out.www_authenticate->value.data = value2.data;
    }

    ctx->head = 1;

    return NGX_OK;
}