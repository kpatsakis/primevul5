fill_yin_extension(struct lys_module *module, struct lyxml_elem *yin, struct lys_ext *ext, struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    const char *value;
    struct lyxml_elem *child, *node, *next, *next2;
    int c_ext = 0, rc;
    void *reallocated;

    GETVAL(ctx, value, yin, "name");

    if (lyp_check_identifier(ctx, value, LY_IDENT_EXTENSION, module, NULL)) {
        goto error;
    }
    ext->name = lydict_insert(ctx, value, strlen(value));

    if (read_yin_common(module, NULL, ext, LYEXT_PAR_EXT, yin, OPT_MODULE, unres)) {
        goto error;
    }

    LY_TREE_FOR_SAFE(yin->child, next, node) {
        if (strcmp(node->ns->value, LY_NSYIN)) {
            /* possible extension instance */
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, ext->ext_size, "extensions", "extension", error);
            c_ext++;
        } else if (!strcmp(node->name, "argument")) {
            /* argument */
            GETVAL(ctx, value, node, "name");
            ext->argument = lydict_insert(ctx, value, strlen(value));
            if (lyp_yin_parse_subnode_ext(module, ext, LYEXT_PAR_EXT, node, LYEXT_SUBSTMT_ARGUMENT, 0, unres)) {
                goto error;
            }

            /* yin-element */
            LY_TREE_FOR_SAFE(node->child, next2, child) {
                if (child->ns == node->ns && !strcmp(child->name, "yin-element")) {
                    GETVAL(ctx, value, child, "value");
                    if (ly_strequal(value, "true", 0)) {
                        ext->flags |= LYS_YINELEM;
                    }

                    if (lyp_yin_parse_subnode_ext(module, ext, LYEXT_PAR_EXT, child, LYEXT_SUBSTMT_YINELEM, 0, unres)) {
                        goto error;
                    }
                } else if (child->ns) {
                    /* unexpected YANG statement */
                    LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, child->name, child->name);
                    goto error;
                } /* else garbage, but save resource needed for unlinking */
            }

            lyxml_free(ctx, node);
        } else {
            /* unexpected YANG statement */
            LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node->name, node->name);
            goto error;
        }
    }

    if (c_ext) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(ext->ext, (c_ext + ext->ext_size) * sizeof *ext->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        ext->ext = reallocated;

        /* init memory */
        memset(&ext->ext[ext->ext_size], 0, c_ext * sizeof *ext->ext);

        /* process the extension instances of the extension itself */
        LY_TREE_FOR_SAFE(yin->child, next, node) {
            rc = lyp_yin_fill_ext(ext, LYEXT_PAR_EXT, 0, 0, module, node, &ext->ext, &ext->ext_size, unres);
            if (rc) {
                goto error;
            }
        }

        lyp_reduce_ext_list(&ext->ext, ext->ext_size, c_ext + ext->ext_size);
    }

    /* search for plugin */
    ext->plugin = ext_get_plugin(ext->name, ext->module->name, ext->module->rev ? ext->module->rev[0].date : NULL);

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}