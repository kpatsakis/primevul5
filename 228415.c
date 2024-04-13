ly_load_plugins(void)
{
    FUN_IN;

    DIR* dir;
    const char *pluginsdir;

#ifdef STATIC
    /* lock the extension plugins list */
    pthread_mutex_lock(&plugins_lock);

    ext_plugins = static_load_lyext_plugins(&ext_plugins_count);
    type_plugins = static_load_lytype_plugins(&type_plugins_count);

    int u;
    for (u = 0; u < static_loaded_plugins_count; u++) {
        ly_add_loaded_plugin(strdup(static_loaded_plugins[u]));
    }

    /* unlock the global structures */
    pthread_mutex_unlock(&plugins_lock);
    return;
#endif /* STATIC */

    /* lock the extension plugins list */
    pthread_mutex_lock(&plugins_lock);

    /* increase references */
    ++plugin_refs;

    /* try to get the plugins directory from environment variable */
    pluginsdir = getenv("LIBYANG_EXTENSIONS_PLUGINS_DIR");
    if (!pluginsdir) {
        pluginsdir = LYEXT_PLUGINS_DIR;
    }

    dir = opendir(pluginsdir);
    if (!dir) {
        /* no directory (or no access to it), no extension plugins */
        LOGWRN(NULL, "Failed to open libyang extensions plugins directory \"%s\" (%s).", pluginsdir, strerror(errno));
    } else {
        ly_load_plugins_dir(dir, pluginsdir, 1);
        closedir(dir);
    }

    /* try to get the plugins directory from environment variable */
    pluginsdir = getenv("LIBYANG_USER_TYPES_PLUGINS_DIR");
    if (!pluginsdir) {
        pluginsdir = LY_USER_TYPES_PLUGINS_DIR;
    }

    dir = opendir(pluginsdir);
    if (!dir) {
        /* no directory (or no access to it), no extension plugins */
        LOGWRN(NULL, "Failed to open libyang user types plugins directory \"%s\" (%s).", pluginsdir, strerror(errno));
    } else {
        ly_load_plugins_dir(dir, pluginsdir, 0);
        closedir(dir);
    }

    /* unlock the global structures */
    pthread_mutex_unlock(&plugins_lock);
}