ly_register_exts(struct lyext_plugin_list *plugin, const char *log_name)
{
    FUN_IN;

    struct lyext_plugin_list *p;
    struct lyext_plugin_complex *pluginc;
    uint32_t u, v;

    for (u = 0; plugin[u].name; u++) {
        /* check extension implementations for collisions */
        for (v = 0; v < ext_plugins_count; v++) {
            if (!strcmp(plugin[u].name, ext_plugins[v].name) &&
                    !strcmp(plugin[u].module, ext_plugins[v].module) &&
                    (!plugin[u].revision || !ext_plugins[v].revision || !strcmp(plugin[u].revision, ext_plugins[v].revision))) {
                LOGERR(NULL, LY_ESYS, "Processing \"%s\" extension plugin failed,"
                        "implementation collision for extension %s from module %s%s%s.",
                        log_name, plugin[u].name, plugin[u].module, plugin[u].revision ? "@" : "",
                        plugin[u].revision ? plugin[u].revision : "");
                return 1;
            }
        }

        /* check for valid supported substatements in case of complex extension */
        if (plugin[u].plugin->type == LYEXT_COMPLEX && ((struct lyext_plugin_complex *)plugin[u].plugin)->substmt) {
            pluginc = (struct lyext_plugin_complex *)plugin[u].plugin;
            for (v = 0; pluginc->substmt[v].stmt; v++) {
                if (pluginc->substmt[v].stmt >= LY_STMT_SUBMODULE ||
                        pluginc->substmt[v].stmt == LY_STMT_VERSION ||
                        pluginc->substmt[v].stmt == LY_STMT_YINELEM) {
                    LOGERR(NULL, LY_EINVAL,
                            "Extension plugin \"%s\" (extension %s) allows not supported extension substatement (%s)",
                            log_name, plugin[u].name, ly_stmt_str[pluginc->substmt[v].stmt]);
                    return 1;
                }
                if (pluginc->substmt[v].cardinality > LY_STMT_CARD_MAND &&
                        pluginc->substmt[v].stmt >= LY_STMT_MODIFIER &&
                        pluginc->substmt[v].stmt <= LY_STMT_STATUS) {
                    LOGERR(NULL, LY_EINVAL, "Extension plugin \"%s\" (extension %s) allows multiple instances on \"%s\" "
                           "substatement, which is not supported.",
                           log_name, plugin[u].name, ly_stmt_str[pluginc->substmt[v].stmt]);
                    return 1;
                }
            }
        }
    }

    /* add the new plugins, we have number of new plugins as u */
    p = realloc(ext_plugins, (ext_plugins_count + u) * sizeof *ext_plugins);
    if (!p) {
        LOGMEM(NULL);
        return -1;
    }
    ext_plugins = p;
    for (; u; u--) {
        memcpy(&ext_plugins[ext_plugins_count], &plugin[u - 1], sizeof *plugin);
        ext_plugins_count++;
    }

    return 0;
}