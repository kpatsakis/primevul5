read_yin_uses(struct lys_module *module, struct lys_node *parent, struct lyxml_elem *yin,
              int options, struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    struct lyxml_elem *sub, *next;
    struct lys_node *retval;
    struct lys_node_uses *uses;
    const char *value;
    int c_ref = 0, c_aug = 0, c_ftrs = 0, c_ext = 0;
    int r;
    void *reallocated;

    uses = calloc(1, sizeof *uses);
    LY_CHECK_ERR_RETURN(!uses, LOGMEM(ctx), NULL);

    uses->nodetype = LYS_USES;
    uses->prev = (struct lys_node *)uses;
    retval = (struct lys_node *)uses;

    GETVAL(ctx, value, yin, "name");
    uses->name = lydict_insert(ctx, value, 0);

    if (read_yin_common(module, parent, retval, LYEXT_PAR_NODE, yin, OPT_MODULE, unres)) {
        goto error;
    }

    LOGDBG(LY_LDGYIN, "parsing %s statement \"%s\"", yin->name, retval->name);

    /* insert the node into the schema tree */
    if (lys_node_addchild(parent, lys_main_module(module), retval, options)) {
        goto error;
    }

    /* get other properties of uses */
    LY_TREE_FOR_SAFE(yin->child, next, sub) {
        if (strcmp(sub->ns->value, LY_NSYIN)) {
            /* extension */
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, retval->ext_size, "extensions", "uses", error);
            c_ext++;
            continue;
        } else if (!strcmp(sub->name, "refine")) {
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ref, uses->refine_size, "refines", "uses", error);
            c_ref++;
        } else if (!strcmp(sub->name, "augment")) {
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_aug, uses->augment_size, "augments", "uses", error);
            c_aug++;
        } else if (!strcmp(sub->name, "if-feature")) {
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ftrs, retval->iffeature_size, "if-features", "uses", error);
            c_ftrs++;
        } else if (!strcmp(sub->name, "when")) {
            if (uses->when) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_LYS, retval, sub->name, yin->name);
                goto error;
            }

            uses->when = read_yin_when(module, sub, unres);
            if (!uses->when) {
                lyxml_free(ctx, sub);
                goto error;
            }
            lyxml_free(ctx, sub);
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_LYS, retval, sub->name);
            goto error;
        }
    }

    /* process properties with cardinality 0..n */
    if (c_ref) {
        uses->refine = calloc(c_ref, sizeof *uses->refine);
        LY_CHECK_ERR_GOTO(!uses->refine, LOGMEM(ctx), error);
    }
    if (c_aug) {
        uses->augment = calloc(c_aug, sizeof *uses->augment);
        LY_CHECK_ERR_GOTO(!uses->augment, LOGMEM(ctx), error);
    }
    if (c_ftrs) {
        uses->iffeature = calloc(c_ftrs, sizeof *uses->iffeature);
        LY_CHECK_ERR_GOTO(!uses->iffeature, LOGMEM(ctx), error);
    }
    if (c_ext) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(retval->ext, (c_ext + retval->ext_size) * sizeof *retval->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        retval->ext = reallocated;

        /* init memory */
        memset(&retval->ext[retval->ext_size], 0, c_ext * sizeof *retval->ext);
    }

    LY_TREE_FOR_SAFE(yin->child, next, sub) {
        if (strcmp(sub->ns->value, LY_NSYIN)) {
            /* extension */
            r = lyp_yin_fill_ext(retval, LYEXT_PAR_NODE, 0, 0, module, sub, &retval->ext, &retval->ext_size, unres);
            if (r) {
                goto error;
            }
        } else if (!strcmp(sub->name, "refine")) {
            r = fill_yin_refine(retval, sub, &uses->refine[uses->refine_size], unres);
            uses->refine_size++;
            if (r) {
                goto error;
            }
        } else if (!strcmp(sub->name, "augment")) {
            r = fill_yin_augment(module, retval, sub, &uses->augment[uses->augment_size], options, unres);
            uses->augment_size++;
            if (r) {
                goto error;
            }
        } else if (!strcmp(sub->name, "if-feature")) {
            r = fill_yin_iffeature(retval, 0, sub, &uses->iffeature[uses->iffeature_size], unres);
            uses->iffeature_size++;
            if (r) {
                goto error;
            }
        }
    }

    lyp_reduce_ext_list(&retval->ext, retval->ext_size, c_ext + retval->ext_size);

    if (unres_schema_add_node(module, unres, uses, UNRES_USES, NULL) == -1) {
        goto error;
    }

    /* check XPath dependencies */
    if (!(ctx->models.flags & LY_CTX_TRUSTED) && uses->when) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax(retval)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, retval, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return retval;

error:
    lys_node_free(ctx, retval, NULL, 0);
    return NULL;
}