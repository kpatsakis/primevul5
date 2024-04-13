static apr_status_t ap_session_set(request_rec * r, session_rec * z,
        const char *key, const char *value)
{
    if (!z) {
        apr_status_t rv;
        rv = ap_session_load(r, &z);
        if (APR_SUCCESS != rv) {
            return rv;
        }
    }
    if (z) {
        if (value) {
            apr_table_set(z->entries, key, value);
        }
        else {
            apr_table_unset(z->entries, key);
        }
        z->dirty = 1;
    }
    return APR_SUCCESS;
}