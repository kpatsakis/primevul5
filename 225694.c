mptcp_convert_dsn(guint64 dsn, mptcp_meta_flow_t *meta, enum mptcp_dsn_conversion conv, gboolean relative, guint64 *result ) {

    *result = dsn;

    /* if relative is set then we need the 64 bits version anyway
     * we assume no wrapping was done on the 32 lsb so this may be wrong for elphant flows
     */
    if(conv == DSN_CONV_32_TO_64 || relative) {

        if(!(meta->static_flags & MPTCP_META_HAS_BASE_DSN_MSB)) {
            /* can't do those without the expected_idsn based on the key */
            return FALSE;
        }
    }

    if(conv == DSN_CONV_32_TO_64) {
        *result = KEEP_32MSB_OF_GUINT64(meta->base_dsn) | dsn;
    }

    if(relative) {
        *result -= meta->base_dsn;
    }

    if(conv == DSN_CONV_64_TO_32) {
        *result = (guint32) *result;
    }

    return TRUE;
}