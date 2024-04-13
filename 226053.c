     set_session_expiry_update(cmd_parms * parms, void *dconf, const char *arg)
{
    session_dir_conf *conf = dconf;

    conf->expiry_update_time = atoi(arg);
    if (conf->expiry_update_time < 0) {
        return "SessionExpiryUpdateInterval must be zero (disable) or a positive value";
    }
    conf->expiry_update_time = apr_time_from_sec(conf->expiry_update_time);
    conf->expiry_update_set = 1;

    return NULL;
}