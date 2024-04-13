ngx_http_auth_spnego_token(
        ngx_http_request_t *r,
        ngx_http_auth_spnego_ctx_t *ctx)
{
    ngx_str_t token;
    ngx_str_t decoded;
    size_t nego_sz = sizeof("Negotiate");

    if (NULL == r->headers_in.authorization) {
        return NGX_DECLINED;
    }

    /* but don't decode second time? */
    if (ctx->token.len)
        return NGX_OK;

    token = r->headers_in.authorization->value;

    if (token.len < nego_sz ||
            ngx_strncasecmp(token.data, (u_char *) "Negotiate ", nego_sz) != 0) {
        if (ngx_strncasecmp(
                    token.data, (u_char *) "NTLM", sizeof("NTLM")) == 0) {
            spnego_log_error("Detected unsupported mechanism: NTLM");
        }
        return NGX_DECLINED;
    }

    token.len -= nego_sz;
    token.data += nego_sz;

    while (token.len && token.data[0] == ' ') {
        token.len--;
        token.data++;
    }

    if (token.len == 0) {
        return NGX_DECLINED;
    }

    decoded.len = ngx_base64_decoded_length(token.len);
    decoded.data = ngx_pnalloc(r->pool, decoded.len);
    if (NULL == decoded.data) {
        return NGX_ERROR;
    }

    if (ngx_decode_base64(&decoded, &token) != NGX_OK) {
        return NGX_DECLINED;
    }

    ctx->token.len = decoded.len;
    ctx->token.data = decoded.data;
    spnego_debug2("Token decoded: %*s", token.len, token.data);

    return NGX_OK;
}