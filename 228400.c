lytype_find(const char *module, const char *revision, const char *type_name)
{
    uint16_t u;

    for (u = 0; u < type_plugins_count; ++u) {
        if (ly_strequal(module, type_plugins[u].module, 0) && ((!revision && !type_plugins[u].revision)
                || (revision && ly_strequal(revision, type_plugins[u].revision, 0)))
                && ly_strequal(type_name, type_plugins[u].name, 0)) {
            return &(type_plugins[u]);
        }
    }

    return NULL;
}