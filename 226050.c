static const char *add_session_exclude(cmd_parms * cmd, void *dconf, const char *f)
{
    session_dir_conf *conf = dconf;

    const char **new = apr_array_push(conf->excludes);
    *new = f;

    return NULL;
}