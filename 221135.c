fill_yin_refine(struct lys_node *uses, struct lyxml_elem *yin, struct lys_refine *rfn, struct unres_schema *unres)
{
    struct ly_ctx *ctx = uses->module->ctx;
    struct lys_module *module;
    struct lyxml_elem *sub, *next;
    const char *value;
    char *endptr;
    int f_mand = 0, f_min = 0, f_max = 0;
    int c_must = 0, c_ftrs = 0, c_dflt = 0, c_ext = 0;
    int r;
    unsigned long int val;
    void *reallocated;

    assert(uses);
    module = uses->module; /* shorthand */

    GETVAL(ctx, value, yin, "target-node");
    rfn->target_name = transform_schema2json(module, value);
    if (!rfn->target_name) {
        goto error;
    }

    LY_TREE_FOR_SAFE(yin->child, next, sub) {
        if (!sub->ns) {
            /* garbage */
        } else if (strcmp(sub->ns->value, LY_NSYIN)) {
            /* extension */
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, rfn->ext_size, "extensions", "refine", error);
            c_ext++;
            continue;

        } else if (!strcmp(sub->name, "description")) {
            if (rfn->dsc) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, yin->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(module, rfn, LYEXT_PAR_REFINE, sub, LYEXT_SUBSTMT_DESCRIPTION, 0, unres)) {
                goto error;
            }

            rfn->dsc = read_yin_subnode(ctx, sub, "text");
            if (!rfn->dsc) {
                goto error;
            }
        } else if (!strcmp(sub->name, "reference")) {
            if (rfn->ref) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, yin->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(module, rfn, LYEXT_PAR_REFINE, sub, LYEXT_SUBSTMT_REFERENCE, 0, unres)) {
                goto error;
            }

            rfn->ref = read_yin_subnode(ctx, sub, "text");
            if (!rfn->ref) {
                goto error;
            }
        } else if (!strcmp(sub->name, "config")) {
            if (rfn->flags & LYS_CONFIG_MASK) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, yin->name);
                goto error;
            }
            GETVAL(ctx, value, sub, "value");
            if (!strcmp(value, "false")) {
                rfn->flags |= LYS_CONFIG_R;
            } else if (!strcmp(value, "true")) {
                rfn->flags |= LYS_CONFIG_W;
            } else {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, sub->name);
                goto error;
            }
            rfn->flags |= LYS_CONFIG_SET;

            if (lyp_yin_parse_subnode_ext(module, rfn, LYEXT_PAR_REFINE, sub, LYEXT_SUBSTMT_CONFIG, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(sub->name, "default")) {
            /* leaf, leaf-list or choice */

            /* check possibility of statements combination */
            if (rfn->target_type) {
                if (c_dflt) {
                    /* multiple defaults are allowed only in leaf-list */
                    if (module->version < 2) {
                        LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, yin->name);
                        goto error;
                    }
                    rfn->target_type &= LYS_LEAFLIST;
                } else {
                    if (module->version < 2) {
                        rfn->target_type &= (LYS_LEAF | LYS_CHOICE);
                    } else {
                        /* YANG 1.1 */
                        rfn->target_type &= (LYS_LEAFLIST | LYS_LEAF | LYS_CHOICE);
                    }
                }
                if (!rfn->target_type) {
                    LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, sub->name, yin->name);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid refine target nodetype for the substatements.");
                    goto error;
                }
            } else {
                if (module->version < 2) {
                    rfn->target_type = LYS_LEAF | LYS_CHOICE;
                } else {
                    /* YANG 1.1 */
                    rfn->target_type = LYS_LEAFLIST | LYS_LEAF | LYS_CHOICE;
                }
            }

            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_dflt, rfn->dflt_size, "defaults", "refine", error);
            if (lyp_yin_parse_subnode_ext(module, rfn, LYEXT_PAR_REFINE, sub, LYEXT_SUBSTMT_DEFAULT, c_dflt, unres)) {
                goto error;
            }
            c_dflt++;
            continue;
        } else if (!strcmp(sub->name, "mandatory")) {
            /* leaf, choice or anyxml */
            if (f_mand) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, yin->name);
                goto error;
            }
            /* just checking the flags in leaf is not sufficient, we would allow
             * multiple mandatory statements with the "false" value
             */
            f_mand = 1;

            /* check possibility of statements combination */
            if (rfn->target_type) {
                rfn->target_type &= (LYS_LEAF | LYS_CHOICE | LYS_ANYDATA);
                if (!rfn->target_type) {
                    LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, sub->name, yin->name);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid refine target nodetype for the substatements.");
                    goto error;
                }
            } else {
                rfn->target_type = LYS_LEAF | LYS_CHOICE | LYS_ANYDATA;
            }

            GETVAL(ctx, value, sub, "value");
            if (!strcmp(value, "true")) {
                rfn->flags |= LYS_MAND_TRUE;
            } else if (!strcmp(value, "false")) {
                rfn->flags |= LYS_MAND_FALSE;
            } else {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, sub->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(module, rfn, LYEXT_PAR_REFINE, sub, LYEXT_SUBSTMT_MANDATORY, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(sub->name, "min-elements")) {
            /* list or leaf-list */
            if (f_min) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, yin->name);
                goto error;
            }
            f_min = 1;

            /* check possibility of statements combination */
            if (rfn->target_type) {
                rfn->target_type &= (LYS_LIST | LYS_LEAFLIST);
                if (!rfn->target_type) {
                    LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, sub->name, yin->name);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid refine target nodetype for the substatements.");
                    goto error;
                }
            } else {
                rfn->target_type = LYS_LIST | LYS_LEAFLIST;
            }

            GETVAL(ctx, value, sub, "value");
            while (isspace(value[0])) {
                value++;
            }

            /* convert it to uint32_t */
            errno = 0;
            endptr = NULL;
            val = strtoul(value, &endptr, 10);
            if (*endptr || value[0] == '-' || errno || val > UINT32_MAX) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, sub->name);
                goto error;
            }
            rfn->mod.list.min = (uint32_t) val;
            rfn->flags |= LYS_RFN_MINSET;

            if (lyp_yin_parse_subnode_ext(module, rfn, LYEXT_PAR_REFINE, sub, LYEXT_SUBSTMT_MIN, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(sub->name, "max-elements")) {
            /* list or leaf-list */
            if (f_max) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, yin->name);
                goto error;
            }
            f_max = 1;

            /* check possibility of statements combination */
            if (rfn->target_type) {
                rfn->target_type &= (LYS_LIST | LYS_LEAFLIST);
                if (!rfn->target_type) {
                    LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, sub->name, yin->name);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid refine target nodetype for the substatements.");
                    goto error;
                }
            } else {
                rfn->target_type = LYS_LIST | LYS_LEAFLIST;
            }

            GETVAL(ctx, value, sub, "value");
            while (isspace(value[0])) {
                value++;
            }

            if (!strcmp(value, "unbounded")) {
                rfn->mod.list.max = 0;
            } else {
                /* convert it to uint32_t */
                errno = 0;
                endptr = NULL;
                val = strtoul(value, &endptr, 10);
                if (*endptr || value[0] == '-' || errno || val == 0 || val > UINT32_MAX) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, sub->name);
                    goto error;
                }
                rfn->mod.list.max = (uint32_t) val;
            }
            rfn->flags |= LYS_RFN_MAXSET;

            if (lyp_yin_parse_subnode_ext(module, rfn, LYEXT_PAR_REFINE, sub, LYEXT_SUBSTMT_MAX, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(sub->name, "presence")) {
            /* container */
            if (rfn->mod.presence) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, sub->name, yin->name);
                goto error;
            }

            /* check possibility of statements combination */
            if (rfn->target_type) {
                rfn->target_type &= LYS_CONTAINER;
                if (!rfn->target_type) {
                    LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, sub->name, yin->name);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid refine target nodetype for the substatements.");
                    goto error;
                }
            } else {
                rfn->target_type = LYS_CONTAINER;
            }

            GETVAL(ctx, value, sub, "value");
            rfn->mod.presence = lydict_insert(ctx, value, strlen(value));

            if (lyp_yin_parse_subnode_ext(module, rfn, LYEXT_PAR_REFINE, sub, LYEXT_SUBSTMT_PRESENCE, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(sub->name, "must")) {
            /* leaf leaf-list, list, container or anyxml */
            /* check possibility of statements combination */
            if (rfn->target_type) {
                rfn->target_type &= (LYS_LEAF | LYS_LIST | LYS_LEAFLIST | LYS_CONTAINER | LYS_ANYDATA);
                if (!rfn->target_type) {
                    LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, sub->name, yin->name);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid refine target nodetype for the substatements.");
                    goto error;
                }
            } else {
                rfn->target_type = LYS_LEAF | LYS_LIST | LYS_LEAFLIST | LYS_CONTAINER | LYS_ANYDATA;
            }

            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_must, rfn->must_size, "musts", "refine", error);
            c_must++;
            continue;

        } else if ((module->version >= 2) && !strcmp(sub->name, "if-feature")) {
            /* leaf, leaf-list, list, container, choice, case, anydata or anyxml */
            /* check possibility of statements combination */
            if (rfn->target_type) {
                rfn->target_type &= (LYS_LEAF | LYS_LIST | LYS_LEAFLIST | LYS_CONTAINER | LYS_CHOICE | LYS_CASE | LYS_ANYDATA);
                if (!rfn->target_type) {
                    LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, sub->name, yin->name);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid refine target nodetype for the substatements.");
                    goto error;
                }
            } else {
                rfn->target_type = LYS_LEAF | LYS_LIST | LYS_LEAFLIST | LYS_CONTAINER | LYS_CHOICE | LYS_CASE | LYS_ANYDATA;
            }

            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ftrs, rfn->iffeature_size, "if-feature", "refine", error);
            c_ftrs++;
            continue;
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, sub->name);
            goto error;
        }

        lyxml_free(ctx, sub);
    }

    /* process nodes with cardinality of 0..n */
    if (c_must) {
        rfn->must = calloc(c_must, sizeof *rfn->must);
        LY_CHECK_ERR_GOTO(!rfn->must, LOGMEM(ctx), error);
    }
    if (c_ftrs) {
        rfn->iffeature = calloc(c_ftrs, sizeof *rfn->iffeature);
        LY_CHECK_ERR_GOTO(!rfn->iffeature, LOGMEM(ctx), error);
    }
    if (c_dflt) {
        rfn->dflt = calloc(c_dflt, sizeof *rfn->dflt);
        LY_CHECK_ERR_GOTO(!rfn->dflt, LOGMEM(ctx), error);
    }
    if (c_ext) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(rfn->ext, (c_ext + rfn->ext_size) * sizeof *rfn->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        rfn->ext = reallocated;

        /* init memory */
        memset(&rfn->ext[rfn->ext_size], 0, c_ext * sizeof *rfn->ext);
    }

    LY_TREE_FOR_SAFE(yin->child, next, sub) {
        if (strcmp(sub->ns->value, LY_NSYIN)) {
            /* extension */
            r = lyp_yin_fill_ext(rfn, LYEXT_PAR_REFINE, 0, 0, module, sub, &rfn->ext, &rfn->ext_size, unres);
            if (r) {
                goto error;
            }
        } else if (!strcmp(sub->name, "if-feature")) {
            r = fill_yin_iffeature(uses, 0, sub, &rfn->iffeature[rfn->iffeature_size], unres);
            rfn->iffeature_size++;
            if (r) {
                goto error;
            }
        } else if (!strcmp(sub->name, "must")) {
            r = fill_yin_must(module, sub, &rfn->must[rfn->must_size], unres);
            rfn->must_size++;
            if (r) {
                goto error;
            }
        } else { /* default */
            GETVAL(ctx, value, sub, "value");

            /* check for duplicity */
            for (r = 0; r < rfn->dflt_size; r++) {
                if (ly_strequal(rfn->dflt[r], value, 1)) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Duplicated default value \"%s\".", value);
                    goto error;
                }
            }
            rfn->dflt[rfn->dflt_size++] = lydict_insert(ctx, value, strlen(value));
        }
    }

    lyp_reduce_ext_list(&rfn->ext, rfn->ext_size, c_ext + rfn->ext_size);

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}