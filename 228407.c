ly_clean_plugins(void)
{
    FUN_IN;

    unsigned int u;
    int ret = EXIT_SUCCESS;

#ifdef STATIC
    /* lock the extension plugins list */
    pthread_mutex_lock(&plugins_lock);

    if(ext_plugins) {
        free(ext_plugins);
        ext_plugins = NULL;
        ext_plugins_count = 0;
    }

    if(type_plugins) {
        free(type_plugins);
        type_plugins = NULL;
        type_plugins_count = 0;
    }

    for (u = 0; u < loaded_plugins_count; ++u) {
        free(loaded_plugins[u]);
    }
    free(loaded_plugins);
    loaded_plugins = NULL;
    loaded_plugins_count = 0;

    /* unlock the global structures */
    pthread_mutex_unlock(&plugins_lock);
    return ret;
#endif /* STATIC */

    /* lock the extension plugins list */
    pthread_mutex_lock(&plugins_lock);

    if (--plugin_refs) {
        /* there is a context that may refer to the plugins, so we cannot remove them */
        ret = EXIT_FAILURE;
        goto cleanup;
    }

    if (!ext_plugins_count && !type_plugins_count) {
        /* no plugin loaded - nothing to do */
        goto cleanup;
    }

    /* clean the lists */
    free(ext_plugins);
    ext_plugins = NULL;
    ext_plugins_count = 0;

    free(type_plugins);
    type_plugins = NULL;
    type_plugins_count = 0;

    for (u = 0; u < loaded_plugins_count; ++u) {
        free(loaded_plugins[u]);
    }
    free(loaded_plugins);
    loaded_plugins = NULL;
    loaded_plugins_count = 0;

    /* close the dl handlers */
    for (u = 0; u < dlhandlers.number; u++) {
        dlclose(dlhandlers.set.g[u]);
    }
    free(dlhandlers.set.g);
    dlhandlers.set.g = NULL;
    dlhandlers.size = 0;
    dlhandlers.number = 0;

cleanup:
    /* unlock the global structures */
    pthread_mutex_unlock(&plugins_lock);

    return ret;
}