fill_yin_feature(struct lys_module *module, struct lyxml_elem *yin, struct lys_feature *f, struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    const char *value;
    struct lyxml_elem *child, *next;
    int c_ftrs = 0, c_ext = 0, ret;
    void *reallocated;

    GETVAL(ctx, value, yin, "name");
    if (lyp_check_identifier(ctx, value, LY_IDENT_FEATURE, module, NULL)) {
        goto error;
    }
    f->name = lydict_insert(ctx, value, strlen(value));
    f->module = module;

    if (read_yin_common(module, NULL, f, LYEXT_PAR_FEATURE, yin, 0, unres)) {
        goto error;
    }

    LY_TREE_FOR(yin->child, child) {
        if (strcmp(child->ns->value, LY_NSYIN)) {
            /* extension */
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, f->ext_size, "extensions", "feature", error);
            c_ext++;
        } else if (!strcmp(child->name, "if-feature")) {
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ftrs, f->iffeature_size, "if-feature", "feature", error);
            c_ftrs++;
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
            goto error;
        }
    }

    if (c_ftrs) {
        f->iffeature = calloc(c_ftrs, sizeof *f->iffeature);
        LY_CHECK_ERR_GOTO(!f->iffeature, LOGMEM(ctx), error);
    }
    if (c_ext) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(f->ext, (c_ext + f->ext_size) * sizeof *f->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        f->ext = reallocated;

        /* init memory */
        memset(&f->ext[f->ext_size], 0, c_ext * sizeof *f->ext);
    }

    LY_TREE_FOR_SAFE(yin->child, next, child) {
        if (strcmp(child->ns->value, LY_NSYIN)) {
            /* extension */
            ret = lyp_yin_fill_ext(f, LYEXT_PAR_FEATURE, 0, 0, module, child, &f->ext, &f->ext_size, unres);
            if (ret) {
                goto error;
            }
        } else { /* if-feature */
            ret = fill_yin_iffeature((struct lys_node *)f, 1, child, &f->iffeature[f->iffeature_size], unres);
            f->iffeature_size++;
            if (ret) {
                goto error;
            }
        }
    }

    lyp_reduce_ext_list(&f->ext, f->ext_size, c_ext + f->ext_size);

    /* check for circular dependencies */
    if (f->iffeature_size) {
        if (unres_schema_add_node(module, unres, f, UNRES_FEATURE, NULL) == -1) {
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}