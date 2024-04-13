fill_yin_import(struct lys_module *module, struct lyxml_elem *yin, struct lys_import *imp, struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    struct lyxml_elem *child, *next, exts;
    const char *value;
    int r, c_ext = 0;
    void *reallocated;

    /* init */
    memset(&exts, 0, sizeof exts);

    LY_TREE_FOR_SAFE(yin->child, next, child) {
        if (!child->ns) {
            /* garbage */
            continue;
        } else if (strcmp(child->ns->value, LY_NSYIN)) {
            /* extension */
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, imp->ext_size, "extensions", "import", error);
            c_ext++;
            lyxml_unlink_elem(ctx, child, 2);
            lyxml_add_child(ctx, &exts, child);
        } else if (!strcmp(child->name, "prefix")) {
            GETVAL(ctx, value, child, "value");
            if (lyp_check_identifier(ctx, value, LY_IDENT_PREFIX, module, NULL)) {
                goto error;
            }
            imp->prefix = lydict_insert(ctx, value, strlen(value));

            if (lyp_yin_parse_subnode_ext(module, imp, LYEXT_PAR_IMPORT, child, LYEXT_SUBSTMT_PREFIX, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(child->name, "revision-date")) {
            if (imp->rev[0]) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            GETVAL(ctx, value, child, "date");
            if (lyp_check_date(ctx, value)) {
                goto error;
            }
            memcpy(imp->rev, value, LY_REV_SIZE - 1);

            if (lyp_yin_parse_subnode_ext(module, imp, LYEXT_PAR_IMPORT, child, LYEXT_SUBSTMT_REVISIONDATE, 0, unres)) {
                goto error;
            }
        } else if ((module->version >= 2) && !strcmp(child->name, "description")) {
            if (imp->dsc) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(module, imp, LYEXT_PAR_IMPORT, child, LYEXT_SUBSTMT_DESCRIPTION, 0, unres)) {
                goto error;
            }
            imp->dsc = read_yin_subnode(ctx, child, "text");
            if (!imp->dsc) {
                goto error;
            }
        } else if ((module->version >= 2) && !strcmp(child->name, "reference")) {
            if (imp->ref) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(module, imp, LYEXT_PAR_IMPORT, child, LYEXT_SUBSTMT_REFERENCE, 0, unres)) {
                goto error;
            }
            imp->ref = read_yin_subnode(ctx, child, "text");
            if (!imp->ref) {
                goto error;
            }
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
            goto error;
        }
    }

    /* check mandatory information */
    if (!imp->prefix) {
        LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "prefix", yin->name);
        goto error;
    }

    /* process extensions */
    if (c_ext) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(imp->ext, (c_ext + imp->ext_size) * sizeof *imp->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        imp->ext = reallocated;

        /* init memory */
        memset(&imp->ext[imp->ext_size], 0, c_ext * sizeof *imp->ext);

        LY_TREE_FOR_SAFE(exts.child, next, child) {
            /* extension */
            r = lyp_yin_fill_ext(imp, LYEXT_PAR_IMPORT, 0, 0, module, child, &imp->ext, &imp->ext_size, unres);
            if (r) {
                goto error;
            }
        }

        lyp_reduce_ext_list(&imp->ext, imp->ext_size, c_ext + imp->ext_size);
    }

    GETVAL(ctx, value, yin, "module");
    return lyp_check_import(module, value, imp);

error:
    while (exts.child) {
        lyxml_free(ctx, exts.child);
    }
    return EXIT_FAILURE;
}