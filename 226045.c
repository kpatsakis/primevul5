static void *create_session_dir_config(apr_pool_t * p, char *dummy)
{
    session_dir_conf *new =
    (session_dir_conf *) apr_pcalloc(p, sizeof(session_dir_conf));

    new->includes = apr_array_make(p, 10, sizeof(const char **));
    new->excludes = apr_array_make(p, 10, sizeof(const char **));

    return (void *) new;
}