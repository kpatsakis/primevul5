static void register_hooks(apr_pool_t * p)
{
    ap_register_output_filter("MOD_SESSION_OUT", session_output_filter,
                              NULL, AP_FTYPE_CONTENT_SET);
    ap_hook_insert_filter(session_insert_output_filter, NULL, NULL,
                          APR_HOOK_MIDDLE);
    ap_hook_insert_error_filter(session_insert_output_filter,
                                NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_fixups(session_fixups, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_session_encode(session_identity_encode, NULL, NULL,
                           APR_HOOK_REALLY_FIRST);
    ap_hook_session_decode(session_identity_decode, NULL, NULL,
                           APR_HOOK_REALLY_LAST);
    APR_REGISTER_OPTIONAL_FN(ap_session_get);
    APR_REGISTER_OPTIONAL_FN(ap_session_set);
    APR_REGISTER_OPTIONAL_FN(ap_session_load);
    APR_REGISTER_OPTIONAL_FN(ap_session_save);
}