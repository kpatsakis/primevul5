fill_yin_include(struct lys_module *module, struct lys_submodule *submodule, struct lyxml_elem *yin,
                 struct lys_include *inc, struct unres_schema *unres)
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
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, inc->ext_size, "extensions", "include", error);
            c_ext++;
            lyxml_unlink_elem(ctx, child, 2);
            lyxml_add_child(ctx, &exts, child);
        } else if (!strcmp(child->name, "revision-date")) {
            if (inc->rev[0]) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "revision-date", yin->name);
                goto error;
            }
            GETVAL(ctx, value, child, "date");
            if (lyp_check_date(ctx, value)) {
                goto error;
            }
            memcpy(inc->rev, value, LY_REV_SIZE - 1);

            if (lyp_yin_parse_subnode_ext(module, inc, LYEXT_PAR_INCLUDE, child, LYEXT_SUBSTMT_REVISIONDATE, 0, unres)) {
                goto error;
            }
        } else if ((module->version >= 2) && !strcmp(child->name, "description")) {
            if (inc->dsc) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(module, inc, LYEXT_PAR_INCLUDE, child, LYEXT_SUBSTMT_DESCRIPTION, 0, unres)) {
                goto error;
            }
            inc->dsc = read_yin_subnode(ctx, child, "text");
            if (!inc->dsc) {
                goto error;
            }
        } else if ((module->version >= 2) && !strcmp(child->name, "reference")) {
            if (inc->ref) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(module, inc, LYEXT_PAR_INCLUDE, child, LYEXT_SUBSTMT_REFERENCE, 0, unres)) {
                goto error;
            }
            inc->ref = read_yin_subnode(ctx, child, "text");
            if (!inc->ref) {
                goto error;
            }
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
            goto error;
        }
    }

    /* process extensions */
    if (c_ext) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(inc->ext, (c_ext + inc->ext_size) * sizeof *inc->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        inc->ext = reallocated;

        /* init memory */
        memset(&inc->ext[inc->ext_size], 0, c_ext * sizeof *inc->ext);

        LY_TREE_FOR_SAFE(exts.child, next, child) {
            /* extension */
            r = lyp_yin_fill_ext(inc, LYEXT_PAR_INCLUDE, 0, 0, module, child, &inc->ext, &inc->ext_size, unres);
            if (r) {
                goto error;
            }
        }

        lyp_reduce_ext_list(&inc->ext, inc->ext_size, c_ext + inc->ext_size);
    }

    GETVAL(ctx, value, yin, "module");
    return lyp_check_include(submodule ? (struct lys_module *)submodule : module, value, inc, unres);

error:
    return -1;
}