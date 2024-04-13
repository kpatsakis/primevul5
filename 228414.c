lyext_load_plugin(void *dlhandler, const char *file_name)
{
    struct lyext_plugin_list *plugin;
    char *str;
    int *version;

#ifdef STATIC
    return 0;
#endif /* STATIC */

    /* get the plugin data */
    plugin = dlsym(dlhandler, file_name);
    str = dlerror();
    if (str) {
        LOGERR(NULL, LY_ESYS, "Processing \"%s\" extension plugin failed, missing plugin list object (%s).", file_name, str);
        return 1;
    }
    version = dlsym(dlhandler, "lyext_api_version");
    if (dlerror() || *version != LYEXT_API_VERSION) {
        LOGWRN(NULL, "Processing \"%s\" extension plugin failed, wrong API version - %d expected, %d found.",
               file_name, LYEXT_API_VERSION, version ? *version : 0);
        return 1;
    }
    return ly_register_exts(plugin, file_name);
}