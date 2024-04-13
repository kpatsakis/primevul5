lytype_free(const struct lys_type *type, lyd_val value, const char *value_str)
{
    struct lytype_plugin_list *p;
    struct lys_node_leaf sleaf;
    struct lyd_node_leaf_list leaf;
    struct lys_module *mod;

    memset(&sleaf, 0, sizeof sleaf);
    memset(&leaf, 0, sizeof leaf);

    while (type->base == LY_TYPE_LEAFREF) {
        type = &type->info.lref.target->type;
    }
    if (type->base == LY_TYPE_UNION) {
        /* create a fake schema node */
        sleaf.module = type->parent->module;
        sleaf.name = "fake-leaf";
        sleaf.type = *type;
        sleaf.nodetype = LYS_LEAF;

        /* and a fake data node */
        leaf.schema = (struct lys_node *)&sleaf;
        leaf.value = value;
        leaf.value_str = value_str;

        /* find the original type */
        type = lyd_leaf_type(&leaf);
        if (!type) {
            LOGINT(sleaf.module->ctx);
            return;
        }
    }

    mod = type->der->module;
    if (!mod) {
        LOGINT(type->parent->module->ctx);
        return;
    }

    p = lytype_find(mod->name, mod->rev_size ? mod->rev[0].date : NULL, type->der->name);
    if (!p) {
        LOGINT(mod->ctx);
        return;
    }

    if (p->free_clb) {
        p->free_clb(value.ptr);
    }
}