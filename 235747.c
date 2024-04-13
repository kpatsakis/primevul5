get_gss_error(
        ngx_pool_t * p,
        OM_uint32 error_status,
        char *prefix)
{
    OM_uint32 maj_stat, min_stat;
    OM_uint32 msg_ctx = 0;
    gss_buffer_desc status_string;
    char buf[1024];
    size_t len;
    ngx_str_t str;
    ngx_snprintf((u_char *) buf, sizeof(buf), "%s: %Z", prefix);
    len = ngx_strlen(buf);
    do {
        maj_stat =
            gss_display_status(&min_stat, error_status, GSS_C_MECH_CODE,
                    GSS_C_NO_OID, &msg_ctx, &status_string);
        if (sizeof(buf) > len + status_string.length + 1) {
            ngx_sprintf((u_char *) buf + len, "%s:%Z",
                    (char *) status_string.value);
            len += (status_string.length + 1);
        }
        gss_release_buffer(&min_stat, &status_string);
    }
    while (!GSS_ERROR(maj_stat) && msg_ctx != 0);

    str.len = len + 1; /* "include" '\0' */
    str.data = (u_char *) buf;
    return (char *) (ngx_pstrdup(p, &str));
}