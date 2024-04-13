fill_yin_augment(struct lys_module *module, struct lys_node *parent, struct lyxml_elem *yin, struct lys_node_augment *aug,
                 int options, struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    const char *value;
    struct lyxml_elem *sub, *next;
    struct lys_node *node;
    int ret, c_ftrs = 0, c_ext = 0;
    void *reallocated;

    aug->nodetype = LYS_AUGMENT;
    GETVAL(ctx, value, yin, "target-node");
    aug->target_name = transform_schema2json(module, value);
    if (!aug->target_name) {
        goto error;
    }
    aug->parent = parent;

    if (read_yin_common(module, NULL, aug, LYEXT_PAR_NODE, yin, OPT_MODULE, unres)) {
        goto error;
    }

    LY_TREE_FOR_SAFE(yin->child, next, sub) {
        if (strcmp(sub->ns->value, LY_NSYIN)) {
            /* extension */
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, aug->ext_size, "extensions", "augment", error);
            c_ext++;
            continue;
        } else if (!strcmp(sub->name, "if-feature")) {
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ftrs, aug->iffeature_size, "if-features", "augment", error);
            c_ftrs++;
            continue;
        } else if (!strcmp(sub->name, "when")) {
            if (aug->when) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, yin->name);
                goto error;
            }

            aug->when = read_yin_when(module, sub, unres);
            if (!aug->when) {
                lyxml_free(ctx, sub);
                goto error;
            }
            lyxml_free(ctx, sub);
            continue;

        /* check allowed data sub-statements */
        } else if (!strcmp(sub->name, "container")) {
            node = read_yin_container(module, (struct lys_node *)aug, sub, options, unres);
        } else if (!strcmp(sub->name, "leaf-list")) {
            node = read_yin_leaflist(module, (struct lys_node *)aug, sub, options, unres);
        } else if (!strcmp(sub->name, "leaf")) {
            node = read_yin_leaf(module, (struct lys_node *)aug, sub, options, unres);
        } else if (!strcmp(sub->name, "list")) {
            node = read_yin_list(module, (struct lys_node *)aug, sub, options, unres);
        } else if (!strcmp(sub->name, "uses")) {
            node = read_yin_uses(module, (struct lys_node *)aug, sub, options, unres);
        } else if (!strcmp(sub->name, "choice")) {
            node = read_yin_choice(module, (struct lys_node *)aug, sub, options, unres);
        } else if (!strcmp(sub->name, "case")) {
            node = read_yin_case(module, (struct lys_node *)aug, sub, options, unres);
        } else if (!strcmp(sub->name, "anyxml")) {
            node = read_yin_anydata(module, (struct lys_node *)aug, sub, LYS_ANYXML, options, unres);
        } else if (!strcmp(sub->name, "anydata")) {
            node = read_yin_anydata(module, (struct lys_node *)aug, sub, LYS_ANYDATA, options, unres);
        } else if (!strcmp(sub->name, "action")) {
            node = read_yin_rpc_action(module, (struct lys_node *)aug, sub, options, unres);
        } else if (!strcmp(sub->name, "notification")) {
            node = read_yin_notif(module, (struct lys_node *)aug, sub, options, unres);
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, sub->name);
            goto error;
        }

        if (!node) {
            goto error;
        }

        node = NULL;
        lyxml_free(ctx, sub);
    }

    if (c_ftrs) {
        aug->iffeature = calloc(c_ftrs, sizeof *aug->iffeature);
        LY_CHECK_ERR_GOTO(!aug->iffeature, LOGMEM(ctx), error);
    }
    if (c_ext) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(aug->ext, (c_ext + aug->ext_size) * sizeof *aug->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        aug->ext = reallocated;

        /* init memory */
        memset(&aug->ext[aug->ext_size], 0, c_ext * sizeof *aug->ext);
    }

    LY_TREE_FOR_SAFE(yin->child, next, sub) {
        if (strcmp(sub->ns->value, LY_NSYIN)) {
            /* extension */
            ret = lyp_yin_fill_ext(aug, LYEXT_PAR_NODE, 0, 0, module, sub, &aug->ext, &aug->ext_size, unres);
            if (ret) {
                goto error;
            }
        } else if (!strcmp(sub->name, "if-feature")) {
            ret = fill_yin_iffeature((struct lys_node *)aug, 0, sub, &aug->iffeature[aug->iffeature_size], unres);
            aug->iffeature_size++;
            if (ret) {
                goto error;
            }
            lyxml_free(ctx, sub);
        }
    }

    lyp_reduce_ext_list(&aug->ext, aug->ext_size, c_ext + aug->ext_size);

    /* aug->child points to the parsed nodes, they must now be
     * connected to the tree and adjusted (if possible right now).
     * However, if this is augment in a uses (parent is NULL), it gets resolved
     * when the uses does and cannot be resolved now for sure
     * (the grouping was not yet copied into uses).
     */
    if (!parent) {
        if (unres_schema_add_node(module, unres, aug, UNRES_AUGMENT, NULL) == -1) {
            goto error;
        }
    }

    /* check XPath dependencies */
    if (!(ctx->models.flags & LY_CTX_TRUSTED) && aug->when) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax((struct lys_node *)aug)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, (struct lys_node *)aug, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}