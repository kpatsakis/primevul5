static int session_included(request_rec * r, session_dir_conf * conf)
{

    const char **includes = (const char **) conf->includes->elts;
    const char **excludes = (const char **) conf->excludes->elts;
    int included = 1;                /* defaults to included */
    int i;

    if (conf->includes->nelts) {
        included = 0;
        for (i = 0; !included && i < conf->includes->nelts; i++) {
            const char *include = includes[i];
            if (strncmp(r->uri, include, strlen(include)) == 0) {
                included = 1;
            }
        }
    }

    if (conf->excludes->nelts) {
        for (i = 0; included && i < conf->excludes->nelts; i++) {
            const char *exclude = excludes[i];
            if (strncmp(r->uri, exclude, strlen(exclude)) == 0) {
                included = 0;
            }
        }
    }

    return included;
}