fill_yin_typedef(struct lys_module *module, struct lys_node *parent, struct lyxml_elem *yin, struct lys_tpdf *tpdf,
                 struct unres_schema *unres)
{
    const char *value;
    struct lyxml_elem *node, *next;
    struct ly_ctx *ctx = module->ctx;
    int rc, has_type = 0, c_ext = 0, i;
    void *reallocated;

    GETVAL(ctx, value, yin, "name");
    if (lyp_check_identifier(ctx, value, LY_IDENT_TYPE, module, parent)) {
        goto error;
    }
    tpdf->name = lydict_insert(ctx, value, strlen(value));

    /* generic part - status, description, reference */
    if (read_yin_common(module, NULL, tpdf, LYEXT_PAR_TPDF, yin, OPT_MODULE, unres)) {
        goto error;
    }

    LY_TREE_FOR_SAFE(yin->child, next, node) {
        if (strcmp(node->ns->value, LY_NSYIN)) {
            /* extension */
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, tpdf->ext_size, "extensions", "typedef", error);
            c_ext++;
            continue;
        } else if (!strcmp(node->name, "type")) {
            if (has_type) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                goto error;
            }
            /* HACK for unres */
            tpdf->type.der = (struct lys_tpdf *)node;
            tpdf->type.parent = tpdf;
            if (unres_schema_add_node(module, unres, &tpdf->type, UNRES_TYPE_DER_TPDF, parent) == -1) {
                goto error;
            }
            has_type = 1;

            /* skip lyxml_free() at the end of the loop, node was freed or at least unlinked in unres processing */
            continue;
        } else if (!strcmp(node->name, "default")) {
            if (tpdf->dflt) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                goto error;
            }
            GETVAL(ctx, value, node, "value");
            tpdf->dflt = lydict_insert(ctx, value, strlen(value));

            if (lyp_yin_parse_subnode_ext(module, tpdf, LYEXT_PAR_TPDF, node, LYEXT_SUBSTMT_DEFAULT, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "units")) {
            if (tpdf->units) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                goto error;
            }
            GETVAL(ctx, value, node, "name");
            tpdf->units = lydict_insert(ctx, value, strlen(value));

            if (lyp_yin_parse_subnode_ext(module, tpdf, LYEXT_PAR_TPDF, node, LYEXT_SUBSTMT_UNITS, 0, unres)) {
                goto error;
            }
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, value);
            goto error;
        }

        lyxml_free(ctx, node);
    }

    /* check mandatory value */
    if (!has_type) {
        LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "type", yin->name);
        goto error;
    }

    /* check default value (if not defined, there still could be some restrictions
     * that need to be checked against a default value from a derived type) */
    if (!(ctx->models.flags & LY_CTX_TRUSTED) &&
            unres_schema_add_node(module, unres, &tpdf->type, UNRES_TYPEDEF_DFLT, (struct lys_node *)(&tpdf->dflt)) == -1) {
        goto error;
    }

    /* finish extensions parsing */
    if (c_ext) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(tpdf->ext, (c_ext + tpdf->ext_size) * sizeof *tpdf->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        tpdf->ext = reallocated;

        /* init memory */
        memset(&tpdf->ext[tpdf->ext_size], 0, c_ext * sizeof *tpdf->ext);

        LY_TREE_FOR_SAFE(yin->child, next, node) {
            rc = lyp_yin_fill_ext(tpdf, LYEXT_PAR_TYPE, 0, 0, module, node, &tpdf->ext, &tpdf->ext_size, unres);
            if (rc) {
                goto error;
            }
        }

        lyp_reduce_ext_list(&tpdf->ext, tpdf->ext_size, c_ext + tpdf->ext_size);
    }

    for (i = 0; i < tpdf->ext_size; ++i) {
        /* set flag, which represent LYEXT_OPT_VALID */
        if (tpdf->ext[i]->flags & LYEXT_OPT_VALID) {
            tpdf->flags |= LYS_VALID_EXT;
            break;
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}