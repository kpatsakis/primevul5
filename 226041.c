static apr_status_t ap_session_get(request_rec * r, session_rec * z,
        const char *key, const char **value)
{
    if (!z) {
        apr_status_t rv;
        rv = ap_session_load(r, &z);
        if (APR_SUCCESS != rv) {
            return rv;
        }
    }
    if (z && z->entries) {
        *value = apr_table_get(z->entries, key);
    }

    return OK;
}