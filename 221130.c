read_yin_common(struct lys_module *module, struct lys_node *parent, void *stmt, LYEXT_PAR stmt_type,
                struct lyxml_elem *xmlnode, int opt, struct unres_schema *unres)
{
    struct lys_node *node = stmt, *p;
    const char *value;
    struct lyxml_elem *sub, *next;
    struct ly_ctx *const ctx = module->ctx;
    char *str;

    if (opt & OPT_MODULE) {
        node->module = module;
    }

    if (opt & OPT_IDENT) {
        GETVAL(ctx, value, xmlnode, "name");
        if (lyp_check_identifier(ctx, value, LY_IDENT_NAME, NULL, NULL)) {
            goto error;
        }
        node->name = lydict_insert(ctx, value, strlen(value));
    }

    /* process local parameters */
    LY_TREE_FOR_SAFE(xmlnode->child, next, sub) {
        if (!sub->ns) {
            /* garbage */
            lyxml_free(ctx, sub);
            continue;
        }
        if  (strcmp(sub->ns->value, LY_NSYIN)) {
            /* possibly an extension, keep the node for later processing, so skipping lyxml_free() */
            continue;
        }

        if (!strcmp(sub->name, "description")) {
            if (node->dsc) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, xmlnode->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(module, stmt, stmt_type, sub, LYEXT_SUBSTMT_DESCRIPTION, 0, unres)) {
                goto error;
            }

            node->dsc = read_yin_subnode(ctx, sub, "text");
            if (!node->dsc) {
                goto error;
            }
        } else if (!strcmp(sub->name, "reference")) {
            if (node->ref) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, xmlnode->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(module, stmt, stmt_type, sub, LYEXT_SUBSTMT_REFERENCE, 0, unres)) {
                goto error;
            }

            node->ref = read_yin_subnode(ctx, sub, "text");
            if (!node->ref) {
                goto error;
            }
        } else if (!strcmp(sub->name, "status")) {
            if (node->flags & LYS_STATUS_MASK) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, xmlnode->name);
                goto error;
            }
            GETVAL(ctx, value, sub, "value");
            if (!strcmp(value, "current")) {
                node->flags |= LYS_STATUS_CURR;
            } else if (!strcmp(value, "deprecated")) {
                node->flags |= LYS_STATUS_DEPRC;
            } else if (!strcmp(value, "obsolete")) {
                node->flags |= LYS_STATUS_OBSLT;
            } else {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, sub->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(module, stmt, stmt_type, sub, LYEXT_SUBSTMT_STATUS, 0, unres)) {
                goto error;
            }
        } else if ((opt & (OPT_CFG_PARSE | OPT_CFG_IGNORE)) && !strcmp(sub->name, "config")) {
            if (opt & OPT_CFG_PARSE) {
                if (node->flags & LYS_CONFIG_MASK) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, xmlnode->name);
                    goto error;
                }
                GETVAL(ctx, value, sub, "value");
                if (!strcmp(value, "false")) {
                    node->flags |= LYS_CONFIG_R;
                } else if (!strcmp(value, "true")) {
                    node->flags |= LYS_CONFIG_W;
                } else {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, sub->name);
                    goto error;
                }
                node->flags |= LYS_CONFIG_SET;

                if (lyp_yin_parse_subnode_ext(module, stmt, stmt_type, sub, LYEXT_SUBSTMT_CONFIG, 0, unres)) {
                    goto error;
                }
            }
        } else {
            /* skip the lyxml_free */
            continue;
        }
        lyxml_free(ctx, sub);
    }

    if ((opt & OPT_CFG_INHERIT) && !(node->flags & LYS_CONFIG_MASK)) {
        /* get config flag from parent */
        if (parent) {
            node->flags |= parent->flags & LYS_CONFIG_MASK;
        } else if (!parent) {
            /* default config is true */
            node->flags |= LYS_CONFIG_W;
        }
    }

    if (parent && (parent->flags & (LYS_STATUS_DEPRC | LYS_STATUS_OBSLT))) {
        /* status is not inherited by specification, but it not make sense to have
         * current in deprecated or deprecated in obsolete, so we print warning
         * and fix the schema by inheriting */
        if (!(node->flags & (LYS_STATUS_MASK))) {
            /* status not explicitely specified on the current node -> inherit */
            if (stmt_type == LYEXT_PAR_NODE) {
                p = node->parent;
                node->parent = parent;
                str = lys_path(node, LYS_PATH_FIRST_PREFIX);
                node->parent = p;
            } else {
                str = lys_path(parent, LYS_PATH_FIRST_PREFIX);
            }
            LOGWRN(ctx, "Missing status in %s subtree (%s), inheriting.",
                   parent->flags & LYS_STATUS_DEPRC ? "deprecated" : "obsolete", str);
            free(str);
            node->flags |= parent->flags & LYS_STATUS_MASK;
        } else if ((parent->flags & LYS_STATUS_MASK) > (node->flags & LYS_STATUS_MASK)) {
            /* invalid combination of statuses */
            switch (node->flags & LYS_STATUS_MASK) {
            case 0:
            case LYS_STATUS_CURR:
                LOGVAL(ctx, LYE_INSTATUS, LY_VLOG_LYS, parent, "current", xmlnode->name, "is child of",
                       parent->flags & LYS_STATUS_DEPRC ? "deprecated" : "obsolete", parent->name);
                break;
            case LYS_STATUS_DEPRC:
                LOGVAL(ctx, LYE_INSTATUS, LY_VLOG_LYS, parent, "deprecated", xmlnode->name, "is child of",
                       "obsolete", parent->name);
                break;
            }
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}