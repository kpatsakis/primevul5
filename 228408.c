lytype_store(const struct lys_module *mod, const char *type_name, const char **value_str, lyd_val *value)
{
    struct lytype_plugin_list *p;
    char *err_msg = NULL;

    assert(mod && type_name && value_str && value);

    p = lytype_find(mod->name, mod->rev_size ? mod->rev[0].date : NULL, type_name);
    if (p) {
        if (p->store_clb(mod->ctx, type_name, value_str, value, &err_msg)) {
            if (!err_msg) {
                if (asprintf(&err_msg, "Failed to store value \"%s\" of user type \"%s\".", *value_str, type_name) == -1) {
                    LOGMEM(mod->ctx);
                    return -1;
                }
            }
            LOGERR(mod->ctx, LY_EPLUGIN, err_msg);
            free(err_msg);
            return -1;
        }

        /* value successfully stored */
        return 0;
    }

    return 1;
}