static const char *add_session_include(cmd_parms * cmd, void *dconf, const char *f)
{
    session_dir_conf *conf = dconf;

    const char **new = apr_array_push(conf->includes);
    *new = f;

    return NULL;
}