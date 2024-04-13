read_sub_module(struct lys_module *module, struct lys_submodule *submodule, struct lyxml_elem *yin,
                struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    struct lyxml_elem *next, *child, root, grps, augs, revs, exts;
    struct lys_node *node = NULL;
    struct lys_module *trg;
    const char *value;
    int i, r, ret = -1;
    int version_flag = 0;
    /* (sub)module substatements are ordered in groups, increment this value when moving to another group
     * 0 - header-stmts, 1 - linkage-stmts, 2 - meta-stmts, 3 - revision-stmts, 4 - body-stmts */
    int substmt_group;
    /* just remember last substatement for logging */
    const char *substmt_prev;
    /* counters */
    int c_imp = 0, c_rev = 0, c_tpdf = 0, c_ident = 0, c_inc = 0, c_aug = 0, c_ftrs = 0, c_dev = 0;
    int c_ext = 0, c_extinst = 0;
    void *reallocated;

    /* to simplify code, store the module/submodule being processed as trg */
    trg = submodule ? (struct lys_module *)submodule : module;

    /* init */
    memset(&root, 0, sizeof root);
    memset(&grps, 0, sizeof grps);
    memset(&augs, 0, sizeof augs);
    memset(&exts, 0, sizeof exts);
    memset(&revs, 0, sizeof revs);

    /*
     * in the first run, we process elements with cardinality of 1 or 0..1 and
     * count elements with cardinality 0..n. Data elements (choices, containers,
     * leafs, lists, leaf-lists) are moved aside to be processed last, since we
     * need have all top-level and groupings already prepared at that time. In
     * the middle loop, we process other elements with carinality of 0..n since
     * we need to allocate arrays to store them.
     */
    substmt_group = 0;
    substmt_prev = NULL;
    LY_TREE_FOR_SAFE(yin->child, next, child) {
        if (!child->ns) {
            /* garbage */
            lyxml_free(ctx, child);
            continue;
        } else if (strcmp(child->ns->value, LY_NSYIN)) {
            /* possible extension instance */
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_extinst, trg->ext_size, "extension instances",
                                          submodule ? "submodule" : "module", error);
            lyxml_unlink_elem(ctx, child, 2);
            lyxml_add_child(ctx, &exts, child);
            c_extinst++;
        } else if (!submodule && !strcmp(child->name, "namespace")) {
            if (substmt_group > 0) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }

            if (trg->ns) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            GETVAL(ctx, value, child, "uri");
            trg->ns = lydict_insert(ctx, value, strlen(value));

            if (lyp_yin_parse_subnode_ext(trg, trg, LYEXT_PAR_MODULE, child, LYEXT_SUBSTMT_NAMESPACE, 0, unres)) {
                goto error;
            }
            lyxml_free(ctx, child);

            substmt_prev = "namespace";
        } else if (!submodule && !strcmp(child->name, "prefix")) {
            if (substmt_group > 0) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }

            if (trg->prefix) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            GETVAL(ctx, value, child, "value");
            if (lyp_check_identifier(ctx, value, LY_IDENT_PREFIX, trg, NULL)) {
                goto error;
            }
            trg->prefix = lydict_insert(ctx, value, strlen(value));

            if (lyp_yin_parse_subnode_ext(trg, trg, LYEXT_PAR_MODULE, child, LYEXT_SUBSTMT_PREFIX, 0, unres)) {
                goto error;
            }
            lyxml_free(ctx, child);

            substmt_prev = "prefix";
        } else if (submodule && !strcmp(child->name, "belongs-to")) {
            if (substmt_group > 0) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }

            if (trg->prefix) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            GETVAL(ctx, value, child, "module");
            if (!ly_strequal(value, submodule->belongsto->name, 1)) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, child->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(trg, trg, LYEXT_PAR_MODULE, child, LYEXT_SUBSTMT_BELONGSTO, 0, unres)) {
                goto error;
            }

            /* get the prefix substatement, start with checks */
            if (!child->child) {
                LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "prefix", child->name);
                goto error;
            } else if (strcmp(child->child->name, "prefix")) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->child->name);
                goto error;
            } else if (child->child->next) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->child->next->name);
                goto error;
            }
            /* and now finally get the value */
            GETVAL(ctx, value, child->child, "value");
            /* check here differs from a generic prefix check, since this prefix
             * don't have to be unique
             */
            if (lyp_check_identifier(ctx, value, LY_IDENT_NAME, NULL, NULL)) {
                goto error;
            }
            submodule->prefix = lydict_insert(ctx, value, strlen(value));

            if (lyp_yin_parse_subnode_ext(trg, trg, LYEXT_PAR_MODULE, child->child, LYEXT_SUBSTMT_PREFIX, 0, unres)) {
                goto error;
            }

            /* we are done with belongs-to */
            lyxml_free(ctx, child);

            substmt_prev = "belongs-to";

            /* counters (statements with n..1 cardinality) */
        } else if (!strcmp(child->name, "import")) {
            if (substmt_group > 1) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }
            substmt_group = 1;
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_imp, trg->imp_size, "imports",
                                          submodule ? "submodule" : "module", error);
            c_imp++;

            substmt_prev = "import";
        } else if (!strcmp(child->name, "revision")) {
            if (substmt_group > 3) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }
            substmt_group = 3;
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_rev, trg->rev_size, "revisions",
                                          submodule ? "submodule" : "module", error);
            c_rev++;

            lyxml_unlink_elem(ctx, child, 2);
            lyxml_add_child(ctx, &revs, child);

            substmt_prev = "revision";
        } else if (!strcmp(child->name, "typedef")) {
            substmt_group = 4;
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_tpdf, trg->tpdf_size, "typedefs",
                                          submodule ? "submodule" : "module", error);
            c_tpdf++;

            substmt_prev = "typedef";
        } else if (!strcmp(child->name, "identity")) {
            substmt_group = 4;
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ident, trg->ident_size, "identities",
                                          submodule ? "submodule" : "module", error);
            c_ident++;

            substmt_prev = "identity";
        } else if (!strcmp(child->name, "include")) {
            if (substmt_group > 1) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }
            substmt_group = 1;
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_inc, trg->inc_size, "includes",
                                          submodule ? "submodule" : "module", error);
            c_inc++;

            substmt_prev = "include";
        } else if (!strcmp(child->name, "augment")) {
            substmt_group = 4;
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_aug, trg->augment_size, "augments",
                                          submodule ? "submodule" : "module", error);
            c_aug++;
            /* keep augments separated, processed last */
            lyxml_unlink_elem(ctx, child, 2);
            lyxml_add_child(ctx, &augs, child);

            substmt_prev = "augment";
        } else if (!strcmp(child->name, "feature")) {
            substmt_group = 4;
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ftrs, trg->features_size, "features",
                                          submodule ? "submodule" : "module", error);
            c_ftrs++;

            substmt_prev = "feature";

            /* data statements */
        } else if (!strcmp(child->name, "container") ||
                !strcmp(child->name, "leaf-list") ||
                !strcmp(child->name, "leaf") ||
                !strcmp(child->name, "list") ||
                !strcmp(child->name, "choice") ||
                !strcmp(child->name, "uses") ||
                !strcmp(child->name, "anyxml") ||
                !strcmp(child->name, "anydata") ||
                !strcmp(child->name, "rpc") ||
                !strcmp(child->name, "notification")) {
            substmt_group = 4;

            lyxml_unlink_elem(ctx, child, 2);
            lyxml_add_child(ctx, &root, child);

            substmt_prev = "data definition";
        } else if (!strcmp(child->name, "grouping")) {
            substmt_group = 4;

            /* keep groupings separated and process them before other data statements */
            lyxml_unlink_elem(ctx, child, 2);
            lyxml_add_child(ctx, &grps, child);

            substmt_prev = "grouping";
            /* optional statements */
        } else if (!strcmp(child->name, "description")) {
            if (substmt_group > 2) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }
            substmt_group = 2;

            if (trg->dsc) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(trg, trg, LYEXT_PAR_MODULE, child, LYEXT_SUBSTMT_DESCRIPTION, 0, unres)) {
                goto error;
            }
            trg->dsc = read_yin_subnode(ctx, child, "text");
            lyxml_free(ctx, child);
            if (!trg->dsc) {
                goto error;
            }

            substmt_prev = "description";
        } else if (!strcmp(child->name, "reference")) {
            if (substmt_group > 2) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }
            substmt_group = 2;

            if (trg->ref) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(trg, trg, LYEXT_PAR_MODULE, child, LYEXT_SUBSTMT_REFERENCE, 0, unres)) {
                goto error;
            }
            trg->ref = read_yin_subnode(ctx, child, "text");
            lyxml_free(ctx, child);
            if (!trg->ref) {
                goto error;
            }

            substmt_prev = "reference";
        } else if (!strcmp(child->name, "organization")) {
            if (substmt_group > 2) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }
            substmt_group = 2;

            if (trg->org) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(trg, trg, LYEXT_PAR_MODULE, child, LYEXT_SUBSTMT_ORGANIZATION, 0, unres)) {
                goto error;
            }
            trg->org = read_yin_subnode(ctx, child, "text");
            lyxml_free(ctx, child);
            if (!trg->org) {
                goto error;
            }

            substmt_prev = "organization";
        } else if (!strcmp(child->name, "contact")) {
            if (substmt_group > 2) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }
            substmt_group = 2;

            if (trg->contact) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(trg, trg, LYEXT_PAR_MODULE, child, LYEXT_SUBSTMT_CONTACT, 0, unres)) {
                goto error;
            }
            trg->contact = read_yin_subnode(ctx, child, "text");
            lyxml_free(ctx, child);
            if (!trg->contact) {
                goto error;
            }

            substmt_prev = "contact";
        } else if (!strcmp(child->name, "yang-version")) {
            if (substmt_group > 0) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Statement \"%s\" cannot appear after \"%s\" statement.",
                       child->name, substmt_prev);
                goto error;
            }

            if (version_flag) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            GETVAL(ctx, value, child, "value");
            if (strcmp(value, "1") && strcmp(value, "1.1")) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "yang-version");
                goto error;
            }
            version_flag = 1;
            if (!strcmp(value, "1")) {
                if (submodule) {
                    if (module->version > 1) {
                        LOGVAL(ctx, LYE_INVER, LY_VLOG_NONE, NULL);
                        goto error;
                    }
                    submodule->version = 1;
                } else {
                    module->version = 1;
                }
            } else {
                if (submodule) {
                    if (module->version < 2) {
                        LOGVAL(ctx, LYE_INVER, LY_VLOG_NONE, NULL);
                        goto error;
                    }
                    submodule->version = 2;
                } else {
                    module->version = 2;
                }
            }

            if (lyp_yin_parse_subnode_ext(trg, trg, LYEXT_PAR_MODULE, child, LYEXT_SUBSTMT_VERSION, 0, unres)) {
                goto error;
            }
            lyxml_free(ctx, child);

            substmt_prev = "yang-version";
        } else if (!strcmp(child->name, "extension")) {
            substmt_group = 4;
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, trg->extensions_size, "extensions",
                                          submodule ? "submodule" : "module", error);
            c_ext++;

            substmt_prev = "extension";
        } else if (!strcmp(child->name, "deviation")) {
            substmt_group = 4;
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_dev, trg->deviation_size, "deviations",
                                          submodule ? "submodule" : "module", error);
            c_dev++;

            substmt_prev = "deviation";
        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
            goto error;
        }
    }

    /* check for mandatory statements */
    if (submodule) {
        if (!submodule->prefix) {
            LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "belongs-to", "submodule");
            goto error;
        }
        if (!version_flag) {
            /* check version compatibility with the main module */
            if (module->version > 1) {
                LOGVAL(ctx, LYE_INVER, LY_VLOG_NONE, NULL);
                goto error;
            }
        }
    } else {
        if (!trg->ns) {
            LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "namespace", "module");
            goto error;
        }
        if (!trg->prefix) {
            LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "prefix", "module");
            goto error;
        }
    }

    /* allocate arrays for elements with cardinality of 0..n */
    if (c_imp) {
        trg->imp = calloc(c_imp, sizeof *trg->imp);
        LY_CHECK_ERR_GOTO(!trg->imp, LOGMEM(ctx), error);
    }
    if (c_rev) {
        trg->rev = calloc(c_rev, sizeof *trg->rev);
        LY_CHECK_ERR_GOTO(!trg->rev, LOGMEM(ctx), error);
    }
    if (c_tpdf) {
        trg->tpdf = calloc(c_tpdf, sizeof *trg->tpdf);
        LY_CHECK_ERR_GOTO(!trg->tpdf, LOGMEM(ctx), error);
    }
    if (c_ident) {
        trg->ident = calloc(c_ident, sizeof *trg->ident);
        LY_CHECK_ERR_GOTO(!trg->ident, LOGMEM(ctx), error);
    }
    if (c_inc) {
        trg->inc = calloc(c_inc, sizeof *trg->inc);
        LY_CHECK_ERR_GOTO(!trg->inc, LOGMEM(ctx), error);
    }
    if (c_aug) {
        trg->augment = calloc(c_aug, sizeof *trg->augment);
        LY_CHECK_ERR_GOTO(!trg->augment, LOGMEM(ctx), error);
    }
    if (c_ftrs) {
        trg->features = calloc(c_ftrs, sizeof *trg->features);
        LY_CHECK_ERR_GOTO(!trg->features, LOGMEM(ctx), error);
    }
    if (c_dev) {
        trg->deviation = calloc(c_dev, sizeof *trg->deviation);
        LY_CHECK_ERR_GOTO(!trg->deviation, LOGMEM(ctx), error);
    }
    if (c_ext) {
        trg->extensions = calloc(c_ext, sizeof *trg->extensions);
        LY_CHECK_ERR_GOTO(!trg->extensions, LOGMEM(ctx), error);
    }

    /* middle part 1 - process revision and then check whether this (sub)module was not already parsed, add it there */
    LY_TREE_FOR_SAFE(revs.child, next, child) {
        r = fill_yin_revision(trg, child, &trg->rev[trg->rev_size], unres);
        trg->rev_size++;
        if (r) {
            goto error;
        }

        /* check uniqueness of the revision date - not required by RFC */
        for (i = 0; i < (trg->rev_size - 1); i++) {
            if (!strcmp(trg->rev[i].date, trg->rev[trg->rev_size - 1].date)) {
                LOGWRN(ctx, "Module's revisions are not unique (%s).", trg->rev[trg->rev_size - 1].date);
                break;
            }
        }

        lyxml_free(ctx, child);
    }

    /* check the module with respect to the context now */
    if (!submodule) {
        switch (lyp_ctx_check_module(module)) {
        case -1:
            goto error;
        case 0:
            break;
        case 1:
            /* it's already there */
            ret = 1;
            goto error;
        }
    }

    /* check first definition of extensions */
    if (c_ext) {
        LY_TREE_FOR_SAFE(yin->child, next, child) {
            if (!strcmp(child->name, "extension")) {
                r = fill_yin_extension(trg, child, &trg->extensions[trg->extensions_size], unres);
                trg->extensions_size++;
                if (r) {
                    goto error;
                }

            }
        }
    }

    /* middle part 2 - process nodes with cardinality of 0..n except the data nodes and augments */
    LY_TREE_FOR_SAFE(yin->child, next, child) {
        if (!strcmp(child->name, "import")) {
            r = fill_yin_import(trg, child, &trg->imp[trg->imp_size], unres);
            trg->imp_size++;
            if (r) {
                goto error;
            }

        } else if (!strcmp(child->name, "include")) {
            r = fill_yin_include(module, submodule, child, &trg->inc[trg->inc_size], unres);
            trg->inc_size++;
            if (r) {
                goto error;
            }

        } else if (!strcmp(child->name, "typedef")) {
            r = fill_yin_typedef(trg, NULL, child, &trg->tpdf[trg->tpdf_size], unres);
            trg->tpdf_size++;
            if (r) {
                goto error;
            }

        } else if (!strcmp(child->name, "identity")) {
            r = fill_yin_identity(trg, child, &trg->ident[trg->ident_size], unres);
            trg->ident_size++;
            if (r) {
                goto error;
            }

        } else if (!strcmp(child->name, "feature")) {
            r = fill_yin_feature(trg, child, &trg->features[trg->features_size], unres);
            trg->features_size++;
            if (r) {
                goto error;
            }

        } else if (!strcmp(child->name, "deviation")) {
            /* must be implemented in this case */
            trg->implemented = 1;

            r = fill_yin_deviation(trg, child, &trg->deviation[trg->deviation_size], unres);
            trg->deviation_size++;
            if (r) {
                goto error;
            }
        }
    }

    /* process extension instances */
    if (c_extinst) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(trg->ext, (c_extinst + trg->ext_size) * sizeof *trg->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        trg->ext = reallocated;

        /* init memory */
        memset(&trg->ext[trg->ext_size], 0, c_extinst * sizeof *trg->ext);

        LY_TREE_FOR_SAFE(exts.child, next, child) {
            r = lyp_yin_fill_ext(trg, LYEXT_PAR_MODULE, 0, 0, trg, child, &trg->ext, &trg->ext_size, unres);
            if (r) {
                goto error;
            }
        }

        lyp_reduce_ext_list(&trg->ext, trg->ext_size, c_ext + trg->ext_size);
    }

    /* process data nodes. Start with groupings to allow uses
     * refer to them. Submodule's data nodes are stored in the
     * main module data tree.
     */
    LY_TREE_FOR_SAFE(grps.child, next, child) {
        node = read_yin_grouping(trg, NULL, child, 0, unres);
        if (!node) {
            goto error;
        }

        lyxml_free(ctx, child);
    }

    /* parse data nodes, ... */
    LY_TREE_FOR_SAFE(root.child, next, child) {

        if (!strcmp(child->name, "container")) {
            node = read_yin_container(trg, NULL, child, 0, unres);
        } else if (!strcmp(child->name, "leaf-list")) {
            node = read_yin_leaflist(trg, NULL, child, 0, unres);
        } else if (!strcmp(child->name, "leaf")) {
            node = read_yin_leaf(trg, NULL, child, 0, unres);
        } else if (!strcmp(child->name, "list")) {
            node = read_yin_list(trg, NULL, child, 0, unres);
        } else if (!strcmp(child->name, "choice")) {
            node = read_yin_choice(trg, NULL, child, 0, unres);
        } else if (!strcmp(child->name, "uses")) {
            node = read_yin_uses(trg, NULL, child, 0, unres);
        } else if (!strcmp(child->name, "anyxml")) {
            node = read_yin_anydata(trg, NULL, child, LYS_ANYXML, 0, unres);
        } else if (!strcmp(child->name, "anydata")) {
            node = read_yin_anydata(trg, NULL, child, LYS_ANYDATA, 0, unres);
        } else if (!strcmp(child->name, "rpc")) {
            node = read_yin_rpc_action(trg, NULL, child, 0, unres);
        } else if (!strcmp(child->name, "notification")) {
            node = read_yin_notif(trg, NULL, child, 0, unres);
        }
        if (!node) {
            goto error;
        }

        lyxml_free(ctx, child);
    }

    /* ... and finally augments (last, so we can augment our data, for instance) */
    LY_TREE_FOR_SAFE(augs.child, next, child) {
        r = fill_yin_augment(trg, NULL, child, &trg->augment[trg->augment_size], 0, unres);
        trg->augment_size++;

        if (r) {
            goto error;
        }
        lyxml_free(ctx, child);
    }

    return 0;

error:
    while (root.child) {
        lyxml_free(ctx, root.child);
    }
    while (grps.child) {
        lyxml_free(ctx, grps.child);
    }
    while (augs.child) {
        lyxml_free(ctx, augs.child);
    }
    while (revs.child) {
        lyxml_free(ctx, revs.child);
    }
    while (exts.child) {
        lyxml_free(ctx, exts.child);
    }

    return ret;
}