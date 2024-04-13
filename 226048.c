     set_session_maxage(cmd_parms * parms, void *dconf, const char *arg)
{
    session_dir_conf *conf = dconf;

    conf->maxage = atol(arg);
    conf->maxage_set = 1;

    return NULL;
}