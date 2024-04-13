     set_session_env(cmd_parms * parms, void *dconf, int flag)
{
    session_dir_conf *conf = dconf;

    conf->env = flag;
    conf->env_set = 1;

    return NULL;
}