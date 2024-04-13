use_keytab(
        ngx_http_request_t * r,
        ngx_str_t *keytab)
{
    size_t kt_sz = keytab->len + 1;
    char *kt = (char *) ngx_pcalloc(r->pool, kt_sz);
    if (NULL == kt) {
        return false;
    }
    ngx_snprintf((u_char *) kt, kt_sz, "%V%Z", keytab);
    OM_uint32 major_status, minor_status = 0;
    major_status = gsskrb5_register_acceptor_identity(kt);
    if (GSS_ERROR(major_status)) {
        spnego_log_error("%s failed to register keytab", get_gss_error(
                    r->pool, minor_status,
                    "gsskrb5_register_acceptor_identity() failed"));
        return false;
    }

    spnego_debug1("Use keytab %V", keytab);
    return true;
}