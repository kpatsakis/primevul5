ngx_http_auth_spnego_set_bogus_authorization(
        ngx_http_request_t *r)
{
    const char *bogus_passwd = "bogus_auth_gss_passwd";
    ngx_str_t plain, encoded, final;

    if (r->headers_in.user.len == 0) {
        spnego_debug0("ngx_http_auth_spnego_set_bogus_authorization: no user NGX_DECLINED");
        return NGX_DECLINED;
    }

    /* +1 because of the ":" in "user:password" */
    plain.len = r->headers_in.user.len + ngx_strlen(bogus_passwd) + 1;
    plain.data = ngx_pnalloc(r->pool, plain.len);
    if (NULL == plain.data) {
        return NGX_ERROR;
    }

    ngx_snprintf(plain.data, plain.len, "%V:%s",
            &r->headers_in.user, bogus_passwd);

    encoded.len = ngx_base64_encoded_length(plain.len);
    encoded.data = ngx_pnalloc(r->pool, encoded.len);
    if (NULL == encoded.data) {
        return NGX_ERROR;
    }

    ngx_encode_base64(&encoded, &plain);

    final.len = sizeof("Basic ") + encoded.len - 1;
    final.data = ngx_pnalloc(r->pool, final.len);
    if (NULL == final.data) {
        return NGX_ERROR;
    }

    ngx_snprintf(final.data, final.len, "Basic %V", &encoded);

    /* WARNING clobbering authorization header value */
    r->headers_in.authorization->value.len = final.len;
    r->headers_in.authorization->value.data = final.data;

    spnego_debug0("ngx_http_auth_spnego_set_bogus_authorization: bogus user set");
    return NGX_OK;
}