     set_session_header(cmd_parms * parms, void *dconf, const char *arg)
{
    session_dir_conf *conf = dconf;

    conf->header = arg;
    conf->header_set = 1;

    return NULL;
}