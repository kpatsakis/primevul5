lytype_load_plugin(void *dlhandler, const char *file_name)
{
    struct lytype_plugin_list *plugin;
    char *str;
    int *version;

#ifdef STATIC
    return 0;
#endif /* STATIC */

    /* get the plugin data */
    plugin = dlsym(dlhandler, file_name);
    str = dlerror();
    if (str) {
        LOGERR(NULL, LY_ESYS, "Processing \"%s\" user type plugin failed, missing plugin list object (%s).", file_name, str);
        return 1;
    }
    version = dlsym(dlhandler, "lytype_api_version");
    if (dlerror() || *version != LYTYPE_API_VERSION) {
        LOGWRN(NULL, "Processing \"%s\" user type plugin failed, wrong API version - %d expected, %d found.",
               file_name, LYTYPE_API_VERSION, version ? *version : 0);
        return 1;
    }
    return ly_register_types(plugin, file_name);
}