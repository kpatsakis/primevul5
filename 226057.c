     set_session_enable(cmd_parms * parms, void *dconf, int flag)
{
    session_dir_conf *conf = dconf;

    conf->enabled = flag;
    conf->enabled_set = 1;

    return NULL;
}