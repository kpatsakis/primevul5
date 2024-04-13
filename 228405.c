ly_register_types(struct lytype_plugin_list *plugin, const char *log_name)
{
    FUN_IN;

    struct lytype_plugin_list *p;
    uint32_t u, v;

    for (u = 0; plugin[u].name; u++) {
        /* check user type implementations for collisions */
        for (v = 0; v < type_plugins_count; v++) {
            if (!strcmp(plugin[u].name, type_plugins[v].name) &&
                    !strcmp(plugin[u].module, type_plugins[v].module) &&
                    (!plugin[u].revision || !type_plugins[v].revision || !strcmp(plugin[u].revision, type_plugins[v].revision))) {
                LOGERR(NULL, LY_ESYS, "Processing \"%s\" extension plugin failed,"
                        "implementation collision for extension %s from module %s%s%s.",
                        log_name, plugin[u].name, plugin[u].module, plugin[u].revision ? "@" : "",
                        plugin[u].revision ? plugin[u].revision : "");
                return 1;
            }
        }
    }

    /* add the new plugins, we have number of new plugins as u */
    p = realloc(type_plugins, (type_plugins_count + u) * sizeof *type_plugins);
    if (!p) {
        LOGMEM(NULL);
        return -1;
    }
    type_plugins = p;
    for (; u; u--) {
        memcpy(&type_plugins[type_plugins_count], &plugin[u - 1], sizeof *plugin);
        type_plugins_count++;
    }

    return 0;
}