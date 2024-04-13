static void *merge_session_dir_config(apr_pool_t * p, void *basev, void *addv)
{
    session_dir_conf *new = (session_dir_conf *) apr_pcalloc(p, sizeof(session_dir_conf));
    session_dir_conf *add = (session_dir_conf *) addv;
    session_dir_conf *base = (session_dir_conf *) basev;

    new->enabled = (add->enabled_set == 0) ? base->enabled : add->enabled;
    new->enabled_set = add->enabled_set || base->enabled_set;
    new->maxage = (add->maxage_set == 0) ? base->maxage : add->maxage;
    new->maxage_set = add->maxage_set || base->maxage_set;
    new->header = (add->header_set == 0) ? base->header : add->header;
    new->header_set = add->header_set || base->header_set;
    new->env = (add->env_set == 0) ? base->env : add->env;
    new->env_set = add->env_set || base->env_set;
    new->includes = apr_array_append(p, base->includes, add->includes);
    new->excludes = apr_array_append(p, base->excludes, add->excludes);
    new->expiry_update_time = (add->expiry_update_set == 0)
                                ? base->expiry_update_time
                                : add->expiry_update_time;
    new->expiry_update_set = add->expiry_update_set || base->expiry_update_set;

    return new;
}