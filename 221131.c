fill_yin_identity(struct lys_module *module, struct lyxml_elem *yin, struct lys_ident *ident, struct unres_schema *unres)
{
    struct lyxml_elem *node, *next;
    struct ly_ctx *ctx = module->ctx;
    const char *value;
    int rc;
    int c_ftrs = 0, c_base = 0, c_ext = 0;
    void *reallocated;

    GETVAL(ctx, value, yin, "name");
    ident->name = value;

    if (read_yin_common(module, NULL, ident, LYEXT_PAR_IDENT, yin, OPT_IDENT | OPT_MODULE, unres)) {
        goto error;
    }

    if (dup_identities_check(ident->name, module)) {
        goto error;
    }

    LY_TREE_FOR(yin->child, node) {
        if (strcmp(node->ns->value, LY_NSYIN)) {
            /* extension */
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, ident->ext_size, "extensions", "identity", error);
            c_ext++;
        } else if (!strcmp(node->name, "base")) {
            if (c_base && (module->version < 2)) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "base", "identity");
                goto error;
            }
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_base, ident->base_size, "bases", "identity", error);
            if (lyp_yin_parse_subnode_ext(module, ident, LYEXT_PAR_IDENT, node, LYEXT_SUBSTMT_BASE, c_base, unres)) {
                goto error;
            }
            c_base++;

        } else if ((module->version >= 2) && !strcmp(node->name, "if-feature")) {
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ftrs, ident->iffeature_size, "if-features", "identity", error);
            c_ftrs++;

        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name, "identity");
            goto error;
        }
    }

    if (c_base) {
        ident->base_size = 0;
        ident->base = calloc(c_base, sizeof *ident->base);
        LY_CHECK_ERR_GOTO(!ident->base, LOGMEM(ctx), error);
    }
    if (c_ftrs) {
        ident->iffeature = calloc(c_ftrs, sizeof *ident->iffeature);
        LY_CHECK_ERR_GOTO(!ident->iffeature, LOGMEM(ctx), error);
    }
    if (c_ext) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(ident->ext, (c_ext + ident->ext_size) * sizeof *ident->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        ident->ext = reallocated;

        /* init memory */
        memset(&ident->ext[ident->ext_size], 0, c_ext * sizeof *ident->ext);
    }

    LY_TREE_FOR_SAFE(yin->child, next, node) {
        if (strcmp(node->ns->value, LY_NSYIN)) {
            /* extension */
            rc = lyp_yin_fill_ext(ident, LYEXT_PAR_IDENT, 0, 0, module, node, &ident->ext, &ident->ext_size, unres);
            if (rc) {
                goto error;
            }
        } else if (!strcmp(node->name, "base")) {
            GETVAL(ctx, value, node, "name");
            value = transform_schema2json(module, value);
            if (!value) {
                goto error;
            }

            if (unres_schema_add_str(module, unres, ident, UNRES_IDENT, value) == -1) {
                lydict_remove(ctx, value);
                goto error;
            }
            lydict_remove(ctx, value);
        } else if (!strcmp(node->name, "if-feature")) {
            rc = fill_yin_iffeature((struct lys_node *)ident, 0, node, &ident->iffeature[ident->iffeature_size], unres);
            ident->iffeature_size++;
            if (rc) {
                goto error;
            }
        }
    }

    lyp_reduce_ext_list(&ident->ext, ident->ext_size, c_ext + ident->ext_size);

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}