EXPORTED void annotate_init(int (*fetch_func)(const char *, const char *,
                                     const strarray_t *, const strarray_t *),
                            int (*store_func)(const char *, const char *,
                                     struct entryattlist *))
{
    if (fetch_func) {
        proxy_fetch_func = fetch_func;
    }
    if (store_func) {
        proxy_store_func = store_func;
    }

    init_annotation_definitions();
    annotate_initialized = 1;
}