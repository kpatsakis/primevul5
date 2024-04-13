ly_load_plugins_dir(DIR *dir, const char *dir_path, int ext_or_type)
{
    struct dirent *file;
    size_t len;
    char *str, *name;
    void *dlhandler;
    int ret;

#ifdef STATIC
    return;
#endif /* STATIC */

    while ((file = readdir(dir))) {
        /* required format of the filename is *LY_PLUGIN_SUFFIX */
        len = strlen(file->d_name);
        if (len < LY_PLUGIN_SUFFIX_LEN + 1 ||
                strcmp(&file->d_name[len - LY_PLUGIN_SUFFIX_LEN], LY_PLUGIN_SUFFIX)) {
            continue;
        }

        /* and construct the filepath */
        if (asprintf(&str, "%s/%s", dir_path, file->d_name) == -1) {
            LOGMEM(NULL);
            return;
        }

        /* load the plugin */
        dlhandler = dlopen(str, RTLD_NOW);
        if (!dlhandler) {
            LOGERR(NULL, LY_ESYS, "Loading \"%s\" as a plugin failed (%s).", str, dlerror());
            free(str);
            continue;
        }
        if (ly_set_contains(&dlhandlers, dlhandler) != -1) {
            /* the plugin is already loaded */
            LOGVRB("Plugin \"%s\" already loaded.", str);
            free(str);

            /* keep the refcount of the shared object correct */
            dlclose(dlhandler);
            continue;
        }
        dlerror();    /* Clear any existing error */

        /* store the name without the suffix */
        name = strndup(file->d_name, len - LY_PLUGIN_SUFFIX_LEN);
        if (!name) {
            LOGMEM(NULL);
            dlclose(dlhandler);
            free(str);
            return;
        }

        if (ext_or_type) {
            ret = lyext_load_plugin(dlhandler, name);
        } else {
            ret = lytype_load_plugin(dlhandler, name);
        }
        if (!ret) {
            LOGVRB("Plugin \"%s\" successfully loaded.", str);
            /* spends name */
            ly_add_loaded_plugin(name);
            /* keep the handler */
            ly_set_add(&dlhandlers, dlhandler, LY_SET_OPT_USEASLIST);
        } else {
            free(name);
            dlclose(dlhandler);
        }
        free(str);

        if (ret == -1) {
            /* finish on error */
            break;
        }
    }
}