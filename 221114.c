fill_yin_deviation(struct lys_module *module, struct lyxml_elem *yin, struct lys_deviation *dev,
                   struct unres_schema *unres)
{
    const char *value, **stritem;
    struct lyxml_elem *next, *next2, *child, *develem;
    int c_dev = 0, c_must, c_uniq, c_dflt, c_ext = 0, c_ext2;
    int f_min = 0, f_max = 0; /* flags */
    int i, j, k = 0, rc;
    unsigned int u;
    struct ly_ctx *ctx = module->ctx;
    struct lys_deviate *d = NULL;
    struct lys_node *node, *parent, *dev_target = NULL;
    struct lys_node_choice *choice = NULL;
    struct lys_node_leaf *leaf = NULL;
    struct ly_set *dflt_check = ly_set_new(), *set;
    struct lys_node_list *list = NULL;
    struct lys_node_leaflist *llist = NULL;
    struct lys_node_inout *inout;
    struct lys_type *t = NULL;
    uint8_t *trg_must_size = NULL;
    struct lys_restr **trg_must = NULL;
    struct unres_schema *tmp_unres;
    struct lys_module *mod;
    void *reallocated;
    size_t deviate_must_index;

    GETVAL(ctx, value, yin, "target-node");
    dev->target_name = transform_schema2json(module, value);
    if (!dev->target_name) {
        goto error;
    }

    /* resolve target node */
    rc = resolve_schema_nodeid(dev->target_name, NULL, module, &set, 0, 1);
    if (rc == -1) {
        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, dev->target_name, yin->name);
        ly_set_free(set);
        goto error;
    }
    dev_target = set->set.s[0];
    ly_set_free(set);

    if (dev_target->module == lys_main_module(module)) {
        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, dev->target_name, yin->name);
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Deviating own module is not allowed.");
        goto error;
    }

    LY_TREE_FOR_SAFE(yin->child, next, child) {
        if (!child->ns ) {
            /* garbage */
            lyxml_free(ctx, child);
            continue;
        } else if (strcmp(child->ns->value, LY_NSYIN)) {
            /* extension */
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, dev->ext_size, "extensions", "deviation", error);
            c_ext++;
            continue;
        } else if (!strcmp(child->name, "description")) {
            if (dev->dsc) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(module, dev, LYEXT_PAR_DEVIATION, child, LYEXT_SUBSTMT_DESCRIPTION, 0, unres)) {
                goto error;
            }
            dev->dsc = read_yin_subnode(ctx, child, "text");
            if (!dev->dsc) {
                goto error;
            }
        } else if (!strcmp(child->name, "reference")) {
            if (dev->ref) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                goto error;
            }
            if (lyp_yin_parse_subnode_ext(module, dev, LYEXT_PAR_DEVIATION, child, LYEXT_SUBSTMT_REFERENCE, 0, unres)) {
                goto error;
            }
            dev->ref = read_yin_subnode(ctx, child, "text");
            if (!dev->ref) {
                goto error;
            }
        } else if (!strcmp(child->name, "deviate")) {
            YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_dev, dev->deviate_size, "deviates", "deviation", error);
            c_dev++;

            /* skip lyxml_free() at the end of the loop, node will be
             * further processed later
             */
            continue;

        } else {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
            goto error;
        }

        lyxml_free(ctx, child);
    }

    if (c_dev) {
        dev->deviate = calloc(c_dev, sizeof *dev->deviate);
        LY_CHECK_ERR_GOTO(!dev->deviate, LOGMEM(ctx), error);
    } else {
        LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "deviate", "deviation");
        goto error;
    }
    if (c_ext) {
        /* some extensions may be already present from the substatements */
        reallocated = realloc(dev->ext, (c_ext + dev->ext_size) * sizeof *dev->ext);
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
        dev->ext = reallocated;

        /* init memory */
        memset(&dev->ext[dev->ext_size], 0, c_ext * sizeof *dev->ext);
    }

    LY_TREE_FOR_SAFE(yin->child, next, develem) {
        if (strcmp(develem->ns->value, LY_NSYIN)) {
            /* deviation's extension */
            rc = lyp_yin_fill_ext(dev, LYEXT_PAR_DEVIATION, 0, 0, module, develem, &dev->ext, &dev->ext_size, unres);
            if (rc) {
                goto error;
            }
            continue;
        }

        /* deviate */
        /* init */
        f_min = 0;
        f_max = 0;
        c_must = 0;
        c_uniq = 0;
        c_dflt = 0;
        c_ext2 = 0;

        /* get deviation type */
        GETVAL(ctx, value, develem, "value");
        if (!strcmp(value, "not-supported")) {
            dev->deviate[dev->deviate_size].mod = LY_DEVIATE_NO;
            /* no other deviate statement is expected,
             * not-supported deviation must be the only deviation of the target
             */
            if (dev->deviate_size || develem->next) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, develem->name);
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "\"not-supported\" deviation cannot be combined with any other deviation.");
                goto error;
            }

            /* you cannot remove a key leaf */
            if ((dev_target->nodetype == LYS_LEAF) && lys_parent(dev_target) && (lys_parent(dev_target)->nodetype == LYS_LIST)) {
                for (i = 0; i < ((struct lys_node_list *)lys_parent(dev_target))->keys_size; ++i) {
                    if (((struct lys_node_list *)lys_parent(dev_target))->keys[i] == (struct lys_node_leaf *)dev_target) {
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, develem->name);
                        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "\"not-supported\" deviation cannot remove a list key.");
                        goto error;
                    }
                }
            }

            /* unlink and store the original node */
            parent = dev_target->parent;
            lys_node_unlink(dev_target);
            if (parent) {
                if (parent->nodetype & (LYS_AUGMENT | LYS_USES)) {
                    /* hack for augment, because when the original will be sometime reconnected back, we actually need
                     * to reconnect it to both - the augment and its target (which is deduced from the deviations target
                     * path), so we need to remember the augment as an addition */
                    /* remember uses parent so we can reconnect to it */
                    dev_target->parent = parent;
                } else if (parent->nodetype & (LYS_RPC | LYS_ACTION)) {
                    /* re-create implicit node */
                    inout = calloc(1, sizeof *inout);
                    LY_CHECK_ERR_GOTO(!inout, LOGMEM(ctx), error);

                    inout->nodetype = dev_target->nodetype;
                    inout->name = lydict_insert(ctx, (inout->nodetype == LYS_INPUT) ? "input" : "output", 0);
                    inout->module = dev_target->module;
                    inout->flags = LYS_IMPLICIT;

                    /* insert it manually */
                    assert(parent->child && !parent->child->next
                           && (parent->child->nodetype == (inout->nodetype == LYS_INPUT ? LYS_OUTPUT : LYS_INPUT)));
                    parent->child->next = (struct lys_node *)inout;
                    inout->prev = parent->child;
                    parent->child->prev = (struct lys_node *)inout;
                    inout->parent = parent;
                }
            }
            dev->orig_node = dev_target;

        } else if (!strcmp(value, "add")) {
            dev->deviate[dev->deviate_size].mod = LY_DEVIATE_ADD;
        } else if (!strcmp(value, "replace")) {
            dev->deviate[dev->deviate_size].mod = LY_DEVIATE_RPL;
        } else if (!strcmp(value, "delete")) {
            dev->deviate[dev->deviate_size].mod = LY_DEVIATE_DEL;
        } else {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, develem->name);
            goto error;
        }
        d = &dev->deviate[dev->deviate_size];
        dev->deviate_size++;

        /* store a shallow copy of the original node */
        if (!dev->orig_node) {
            tmp_unres = calloc(1, sizeof *tmp_unres);
            dev->orig_node = lys_node_dup(dev_target->module, NULL, dev_target, tmp_unres, 1);
            /* such a case is not really supported but whatever */
            unres_schema_free(dev_target->module, &tmp_unres, 1);
        }

        /* process deviation properties */
        LY_TREE_FOR_SAFE(develem->child, next2, child) {
            if (!child->ns) {
                /* garbage */
                lyxml_free(ctx, child);
                continue;
            } else if  (strcmp(child->ns->value, LY_NSYIN)) {
                /* extensions */
                YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext2, d->ext_size, "extensions", "deviate", error);
                c_ext2++;
            } else if (d->mod == LY_DEVIATE_NO) {
                /* no YIN substatement expected in this case */
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                goto error;
            } else if (!strcmp(child->name, "config")) {
                if (d->flags & LYS_CONFIG_MASK) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                    goto error;
                }

                /* for we deviate from RFC 6020 and allow config property even it is/is not
                 * specified in the target explicitly since config property inherits. So we expect
                 * that config is specified in every node. But for delete, we check that the value
                 * is the same as here in deviation
                 */
                GETVAL(ctx, value, child, "value");
                if (!strcmp(value, "false")) {
                    d->flags |= LYS_CONFIG_R;
                } else if (!strcmp(value, "true")) {
                    d->flags |= LYS_CONFIG_W;
                } else {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, child->name);
                    goto error;
                }

                if (d->mod == LY_DEVIATE_DEL) {
                    /* del config is forbidden */
                    LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "config", "deviate delete");
                    goto error;
                } else if ((d->mod == LY_DEVIATE_ADD) && (dev_target->flags & LYS_CONFIG_SET)) {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "config");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Adding property that already exists.");
                    goto error;
                } else if ((d->mod == LY_DEVIATE_RPL) && !(dev_target->flags & LYS_CONFIG_SET)) {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "config");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Replacing a property that does not exist.");
                    goto error;
                } else { /* add and replace are the same in this case */
                    /* remove current config value of the target ... */
                    dev_target->flags &= ~LYS_CONFIG_MASK;

                    /* ... and replace it with the value specified in deviation */
                    dev_target->flags |= d->flags & LYS_CONFIG_MASK;
                }

                if (lyp_yin_parse_subnode_ext(module, d, LYEXT_PAR_DEVIATE, child, LYEXT_SUBSTMT_CONFIG, 0, unres)) {
                    goto error;
                }
            } else if (!strcmp(child->name, "default")) {
                if (lyp_yin_parse_subnode_ext(module, d, LYEXT_PAR_DEVIATE, child, LYEXT_SUBSTMT_DEFAULT, c_dflt, unres)) {
                    goto error;
                }
                YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_dflt, d->dflt_size, "defaults", "deviate", error);
                c_dflt++;

                /* check target node type */
                if (module->version < 2 && dev_target->nodetype == LYS_LEAFLIST) {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "default");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"default\" property.");
                    goto error;
                } else if (c_dflt > 1 && dev_target->nodetype != LYS_LEAFLIST) { /* from YANG 1.1 */
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "default");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow multiple \"default\" properties.");
                    goto error;
                } else if (c_dflt == 1 && (!(dev_target->nodetype & (LYS_LEAF | LYS_LEAFLIST | LYS_CHOICE)))) {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "default");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"default\" property.");
                    goto error;
                }

                /* skip lyxml_free() at the end of the loop, this node will be processed later */
                continue;

            } else if (!strcmp(child->name, "mandatory")) {
                if (d->flags & LYS_MAND_MASK) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                    goto error;
                }

                /* check target node type */
                if (!(dev_target->nodetype & (LYS_LEAF | LYS_CHOICE | LYS_ANYDATA))) {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"%s\" property.", child->name);
                    goto error;
                }

                GETVAL(ctx, value, child, "value");
                if (!strcmp(value, "false")) {
                    d->flags |= LYS_MAND_FALSE;
                } else if (!strcmp(value, "true")) {
                    d->flags |= LYS_MAND_TRUE;
                } else {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, child->name);
                    goto error;
                }

                if (d->mod == LY_DEVIATE_ADD) {
                    /* check that there is no current value */
                    if (dev_target->flags & LYS_MAND_MASK) {
                        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Adding property that already exists.");
                        goto error;
                    }

                    /* check collision with default-stmt */
                    if (d->flags & LYS_MAND_TRUE) {
                        if (dev_target->nodetype == LYS_CHOICE) {
                            if (((struct lys_node_choice *)(dev_target))->dflt) {
                                LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, child->name, child->parent->name);
                                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL,
                                       "Adding the \"mandatory\" statement is forbidden on choice with the \"default\" statement.");
                                goto error;
                            }
                        } else if (dev_target->nodetype == LYS_LEAF) {
                            if (((struct lys_node_leaf *)(dev_target))->dflt) {
                                LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, child->name, child->parent->name);
                                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL,
                                       "Adding the \"mandatory\" statement is forbidden on leaf with the \"default\" statement.");
                                goto error;
                            }
                        }
                    }

                    dev_target->flags |= d->flags & LYS_MAND_MASK;
                } else if (d->mod == LY_DEVIATE_RPL) {
                    /* check that there was a value before */
                    if (!(dev_target->flags & LYS_MAND_MASK)) {
                        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Replacing a property that does not exist.");
                        goto error;
                    }

                    dev_target->flags &= ~LYS_MAND_MASK;
                    dev_target->flags |= d->flags & LYS_MAND_MASK;
                } else if (d->mod == LY_DEVIATE_DEL) {
                    /* del mandatory is forbidden */
                    LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "mandatory", "deviate delete");
                    goto error;
                }

                /* check for mandatory node in default case, first find the closest parent choice to the changed node */
                for (parent = dev_target->parent;
                     parent && !(parent->nodetype & (LYS_CHOICE | LYS_GROUPING | LYS_ACTION));
                     parent = parent->parent) {
                    if (parent->nodetype == LYS_CONTAINER && ((struct lys_node_container *)parent)->presence) {
                        /* stop also on presence containers */
                        break;
                    }
                }
                /* and if it is a choice with the default case, check it for presence of a mandatory node in it */
                if (parent && parent->nodetype == LYS_CHOICE && ((struct lys_node_choice *)parent)->dflt) {
                    if (lyp_check_mandatory_choice(parent)) {
                        goto error;
                    }
                }

                if (lyp_yin_parse_subnode_ext(module, d, LYEXT_PAR_DEVIATE, child, LYEXT_SUBSTMT_MANDATORY, 0, unres)) {
                    goto error;
                }
            } else if (!strcmp(child->name, "min-elements")) {
                if (f_min) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                    goto error;
                }
                f_min = 1;

                if (deviate_minmax(dev_target, child, d, 0)) {
                    goto error;
                }
                if (lyp_yin_parse_subnode_ext(module, d, LYEXT_PAR_DEVIATE, child, LYEXT_SUBSTMT_MIN, 0, unres)) {
                    goto error;
                }
            } else if (!strcmp(child->name, "max-elements")) {
                if (f_max) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                    goto error;
                }
                f_max = 1;

                if (deviate_minmax(dev_target, child, d, 1)) {
                    goto error;
                }
                if (lyp_yin_parse_subnode_ext(module, d, LYEXT_PAR_DEVIATE, child, LYEXT_SUBSTMT_MAX, 0, unres)) {
                    goto error;
                }
            } else if (!strcmp(child->name, "must")) {
                YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_must, d->must_size, "musts", "deviate", error);
                c_must++;
                /* skip lyxml_free() at the end of the loop, this node will be processed later */
                continue;
            } else if (!strcmp(child->name, "type")) {
                if (d->type) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                    goto error;
                }

                /* add, del type is forbidden */
                if (d->mod == LY_DEVIATE_ADD) {
                    LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "type", "deviate add");
                    goto error;
                } else if (d->mod == LY_DEVIATE_DEL) {
                    LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "type", "deviate delete");
                    goto error;
                }

                /* check target node type */
                if (dev_target->nodetype == LYS_LEAF) {
                    t = &((struct lys_node_leaf *)dev_target)->type;
                    if (((struct lys_node_leaf *)dev_target)->dflt) {
                        ly_set_add(dflt_check, dev_target, 0);
                    }
                } else if (dev_target->nodetype == LYS_LEAFLIST) {
                    t = &((struct lys_node_leaflist *)dev_target)->type;
                    if (((struct lys_node_leaflist *)dev_target)->dflt) {
                        ly_set_add(dflt_check, dev_target, 0);
                    }
                } else {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"%s\" property.", child->name);
                    goto error;
                }

                /* replace */
                lys_type_free(ctx, t, NULL);
                memset(t, 0, sizeof (struct lys_type));
                /* HACK for unres */
                t->der = (struct lys_tpdf *)child;
                t->parent = (struct lys_tpdf *)dev_target;
                if (unres_schema_add_node(module, unres, t, UNRES_TYPE_DER, dev_target) == -1) {
                    goto error;
                }
                d->type = t;
            } else if (!strcmp(child->name, "unique")) {
                if (lyp_yin_parse_subnode_ext(module, d, LYEXT_PAR_DEVIATE, child, LYEXT_SUBSTMT_UNIQUE, c_uniq, unres)) {
                    goto error;
                }
                YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_uniq, d->unique_size, "uniques", "deviate", error);
                c_uniq++;
                /* skip lyxml_free() at the end of the loop, this node will be processed later */
                continue;
            } else if (!strcmp(child->name, "units")) {
                if (d->units) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, child->name, yin->name);
                    goto error;
                }

                /* check target node type */
                if (dev_target->nodetype == LYS_LEAFLIST) {
                    stritem = &((struct lys_node_leaflist *)dev_target)->units;
                } else if (dev_target->nodetype == LYS_LEAF) {
                    stritem = &((struct lys_node_leaf *)dev_target)->units;
                } else {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"%s\" property.", child->name);
                    goto error;
                }

                /* get units value */
                GETVAL(ctx, value, child, "name");
                d->units = lydict_insert(ctx, value, 0);

                /* apply to target */
                if (d->mod == LY_DEVIATE_ADD) {
                    /* check that there is no current value */
                    if (*stritem) {
                        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Adding property that already exists.");
                        goto error;
                    }

                    *stritem = lydict_insert(ctx, value, 0);
                } else if (d->mod == LY_DEVIATE_RPL) {
                    /* check that there was a value before */
                    if (!*stritem) {
                        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Replacing a property that does not exist.");
                        goto error;
                    }

                    lydict_remove(ctx, *stritem);
                    *stritem = lydict_insert(ctx, value, 0);
                } else if (d->mod == LY_DEVIATE_DEL) {
                    /* check values */
                    if (!ly_strequal(*stritem, d->units, 1)) {
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, child->name);
                        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Value differs from the target being deleted.");
                        goto error;
                    }
                    /* remove current units value of the target */
                    lydict_remove(ctx, *stritem);
                    (*stritem) = NULL;

                    /* remove its extensions */
                    j = -1;
                    while ((j = lys_ext_iter(dev_target->ext, dev_target->ext_size, j + 1, LYEXT_SUBSTMT_UNITS)) != -1) {
                        lyp_ext_instance_rm(ctx, &dev_target->ext, &dev_target->ext_size, j);
                        --j;
                    }
                }

                if (lyp_yin_parse_subnode_ext(module, d, LYEXT_PAR_DEVIATE, child, LYEXT_SUBSTMT_UNITS, 0, unres)) {
                    goto error;
                }
            } else {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, child->name);
                goto error;
            }

            /* do not free sub, it could have been unlinked and stored in unres */
        }

        if (c_must) {
            /* check target node type */
            switch (dev_target->nodetype) {
            case LYS_LEAF:
                trg_must = &((struct lys_node_leaf *)dev_target)->must;
                trg_must_size = &((struct lys_node_leaf *)dev_target)->must_size;
                break;
            case LYS_CONTAINER:
                trg_must = &((struct lys_node_container *)dev_target)->must;
                trg_must_size = &((struct lys_node_container *)dev_target)->must_size;
                break;
            case LYS_LEAFLIST:
                trg_must = &((struct lys_node_leaflist *)dev_target)->must;
                trg_must_size = &((struct lys_node_leaflist *)dev_target)->must_size;
                break;
            case LYS_LIST:
                trg_must = &((struct lys_node_list *)dev_target)->must;
                trg_must_size = &((struct lys_node_list *)dev_target)->must_size;
                break;
            case LYS_ANYXML:
            case LYS_ANYDATA:
                trg_must = &((struct lys_node_anydata *)dev_target)->must;
                trg_must_size = &((struct lys_node_anydata *)dev_target)->must_size;
                break;
            default:
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "must");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"must\" property.");
                goto error;
            }

            dev_target->flags &= ~(LYS_XPCONF_DEP | LYS_XPSTATE_DEP);

            if (d->mod == LY_DEVIATE_RPL) {
                /* replace must is forbidden */
                LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "must", "deviate replace");
                goto error;
            } else if (d->mod == LY_DEVIATE_ADD) {
                /* reallocate the must array of the target */
                struct lys_restr *must = ly_realloc(*trg_must, (c_must + *trg_must_size) * sizeof *d->must);
                LY_CHECK_ERR_GOTO(!must, LOGMEM(ctx), error);
                *trg_must = must;
                d->must = calloc(c_must, sizeof *d->must);
                d->must_size = c_must;
            } else { /* LY_DEVIATE_DEL */
                d->must = calloc(c_must, sizeof *d->must);
            }
            LY_CHECK_ERR_GOTO(!d->must, LOGMEM(ctx), error);
        }
        if (c_uniq) {
            /* replace unique is forbidden */
            if (d->mod == LY_DEVIATE_RPL) {
                LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "unique", "deviate replace");
                goto error;
            }

            /* check target node type */
            if (dev_target->nodetype != LYS_LIST) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "unique");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"unique\" property.");
                goto error;
            }

            list = (struct lys_node_list *)dev_target;
            if (d->mod == LY_DEVIATE_ADD) {
                /* reallocate the unique array of the target */
                d->unique = ly_realloc(list->unique, (c_uniq + list->unique_size) * sizeof *d->unique);
                LY_CHECK_ERR_GOTO(!d->unique, LOGMEM(ctx), error);
                list->unique = d->unique;
                d->unique = &list->unique[list->unique_size];
                d->unique_size = c_uniq;
            } else { /* LY_DEVIATE_DEL */
                d->unique = calloc(c_uniq, sizeof *d->unique);
                LY_CHECK_ERR_GOTO(!d->unique, LOGMEM(ctx), error);
            }
        }
        if (c_dflt) {
            if (d->mod == LY_DEVIATE_ADD) {
                /* check that there is no current value */
                if ((dev_target->nodetype == LYS_LEAF && ((struct lys_node_leaf *)dev_target)->dflt) ||
                        (dev_target->nodetype == LYS_CHOICE && ((struct lys_node_choice *)dev_target)->dflt)) {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "default");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Adding property that already exists.");
                    goto error;
                }

                /* check collision with mandatory/min-elements */
                if ((dev_target->flags & LYS_MAND_TRUE) ||
                        (dev_target->nodetype == LYS_LEAFLIST && ((struct lys_node_leaflist *)dev_target)->min)) {
                    LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "default", "deviation");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL,
                           "Adding the \"default\" statement is forbidden on %s statement.",
                           (dev_target->flags & LYS_MAND_TRUE) ? "nodes with the \"mandatory\"" : "leaflists with non-zero \"min-elements\"");
                    goto error;
                }
            } else if (d->mod == LY_DEVIATE_RPL) {
                /* check that there was a value before */
                if (((dev_target->nodetype & (LYS_LEAF | LYS_LEAFLIST)) && !((struct lys_node_leaf *)dev_target)->dflt) ||
                        (dev_target->nodetype == LYS_CHOICE && !((struct lys_node_choice *)dev_target)->dflt)) {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "default");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Replacing a property that does not exist.");
                    goto error;
                }
            }

            if (dev_target->nodetype == LYS_LEAFLIST) {
                /* reallocate default list in the target */
                llist = (struct lys_node_leaflist *)dev_target;
                if (d->mod == LY_DEVIATE_ADD) {
                    /* reallocate (enlarge) the unique array of the target */
                    llist->dflt = ly_realloc(llist->dflt, (c_dflt + llist->dflt_size) * sizeof *d->dflt);
                    LY_CHECK_ERR_GOTO(!llist->dflt, LOGMEM(ctx), error);
                } else if (d->mod == LY_DEVIATE_RPL) {
                    /* reallocate (replace) the unique array of the target */
                    for (i = 0; i < llist->dflt_size; i++) {
                        lydict_remove(ctx, llist->dflt[i]);
                    }
                    llist->dflt = ly_realloc(llist->dflt, c_dflt * sizeof *d->dflt);
                    llist->dflt_size = 0;
                    LY_CHECK_ERR_GOTO(!llist->dflt, LOGMEM(ctx), error);
                }
            }
            d->dflt = calloc(c_dflt, sizeof *d->dflt);
            LY_CHECK_ERR_GOTO(!d->dflt, LOGMEM(ctx), error);
        }
        if (c_ext2) {
            /* some extensions may be already present from the substatements */
            reallocated = realloc(d->ext, (c_ext2 + d->ext_size) * sizeof *d->ext);
            LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(ctx), error);
            d->ext = reallocated;

            /* init memory */
            memset(&d->ext[d->ext_size], 0, c_ext2 * sizeof *d->ext);
        }

        /* process deviation properties with 0..n cardinality */
        deviate_must_index = 0;
        LY_TREE_FOR_SAFE(develem->child, next2, child) {
            if (strcmp(child->ns->value, LY_NSYIN)) {
                /* extension */
                if (lyp_yin_fill_ext(d, LYEXT_PAR_DEVIATE, 0, 0, module, child, &d->ext, &d->ext_size, unres)) {
                    goto error;
                }
            } else if (!strcmp(child->name, "must")) {
                if (d->mod == LY_DEVIATE_DEL) {
                    if (fill_yin_must(module, child, &d->must[d->must_size], unres)) {
                        goto error;
                    }

                    /* find must to delete, we are ok with just matching conditions */
                    for (i = 0; i < *trg_must_size; i++) {
                        if (ly_strequal(d->must[d->must_size].expr, (*trg_must)[i].expr, 1)) {
                            /* we have a match, free the must structure ... */
                            lys_restr_free(ctx, &((*trg_must)[i]), NULL);
                            /* ... and maintain the array */
                            (*trg_must_size)--;
                            if (i != *trg_must_size) {
                                (*trg_must)[i].expr = (*trg_must)[*trg_must_size].expr;
                                (*trg_must)[i].dsc = (*trg_must)[*trg_must_size].dsc;
                                (*trg_must)[i].ref = (*trg_must)[*trg_must_size].ref;
                                (*trg_must)[i].eapptag = (*trg_must)[*trg_must_size].eapptag;
                                (*trg_must)[i].emsg = (*trg_must)[*trg_must_size].emsg;
                            }
                            if (!(*trg_must_size)) {
                                free(*trg_must);
                                *trg_must = NULL;
                            } else {
                                (*trg_must)[*trg_must_size].expr = NULL;
                                (*trg_must)[*trg_must_size].dsc = NULL;
                                (*trg_must)[*trg_must_size].ref = NULL;
                                (*trg_must)[*trg_must_size].eapptag = NULL;
                                (*trg_must)[*trg_must_size].emsg = NULL;
                            }

                            i = -1; /* set match flag */
                            break;
                        }
                    }
                    d->must_size++;
                    if (i != -1) {
                        /* no match found */
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL,
                               d->must[d->must_size - 1].expr, child->name);
                        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Value does not match any must from the target.");
                        goto error;
                    }
                } else { /* replace or add */
                    memset(&((*trg_must)[*trg_must_size]), 0, sizeof **trg_must);
                    if (fill_yin_must(module, child, &((*trg_must)[*trg_must_size]), unres)) {
                        goto error;
                    }
                    memcpy(d->must + deviate_must_index, &((*trg_must)[*trg_must_size]), sizeof *d->must);
                    ++deviate_must_index;
                    (*trg_must_size)++;
                }

                /* check XPath dependencies again */
                if (*trg_must_size && !(ctx->models.flags & LY_CTX_TRUSTED) &&
                        (unres_schema_add_node(module, unres, dev_target, UNRES_XPATH, NULL) == -1)) {
                    goto error;
                }
            } else if (!strcmp(child->name, "unique")) {
                if (d->mod == LY_DEVIATE_DEL) {
                    memset(&d->unique[d->unique_size], 0, sizeof *d->unique);
                    if (fill_yin_unique(module, dev_target, child, &d->unique[d->unique_size], NULL)) {
                        d->unique_size++;
                        goto error;
                    }

                    /* find unique structures to delete */
                    for (i = 0; i < list->unique_size; i++) {
                        if (list->unique[i].expr_size != d->unique[d->unique_size].expr_size) {
                            continue;
                        }

                        for (j = 0; j < d->unique[d->unique_size].expr_size; j++) {
                            if (!ly_strequal(list->unique[i].expr[j], d->unique[d->unique_size].expr[j], 1)) {
                                break;
                            }
                        }

                        if (j == d->unique[d->unique_size].expr_size) {
                            /* we have a match, free the unique structure ... */
                            for (j = 0; j < list->unique[i].expr_size; j++) {
                                lydict_remove(ctx, list->unique[i].expr[j]);
                            }
                            free(list->unique[i].expr);
                            /* ... and maintain the array */
                            list->unique_size--;
                            if (i != list->unique_size) {
                                list->unique[i].expr_size = list->unique[list->unique_size].expr_size;
                                list->unique[i].expr = list->unique[list->unique_size].expr;
                            }

                            if (!list->unique_size) {
                                free(list->unique);
                                list->unique = NULL;
                            } else {
                                list->unique[list->unique_size].expr_size = 0;
                                list->unique[list->unique_size].expr = NULL;
                            }

                            k = i; /* remember index for removing extensions */
                            i = -1; /* set match flag */
                            break;
                        }
                    }

                    d->unique_size++;
                    if (i != -1) {
                        /* no match found */
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, lyxml_get_attr(child, "tag", NULL), child->name);
                        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Value differs from the target being deleted.");
                        goto error;
                    }

                    /* remove extensions of this unique instance from the target node */
                    j = -1;
                    while ((j = lys_ext_iter(dev_target->ext, dev_target->ext_size, j + 1, LYEXT_SUBSTMT_UNIQUE)) != -1) {
                        if (dev_target->ext[j]->insubstmt_index == k) {
                            lyp_ext_instance_rm(ctx, &dev_target->ext, &dev_target->ext_size, j);
                            --j;
                        } else if (dev_target->ext[j]->insubstmt_index > k) {
                            /* decrease the substatement index of the extension because of the changed array of uniques */
                            dev_target->ext[j]->insubstmt_index--;
                        }
                    }
                } else { /* replace or add */
                    memset(&list->unique[list->unique_size], 0, sizeof *list->unique);
                    i = fill_yin_unique(module, dev_target, child, &list->unique[list->unique_size], NULL);
                    list->unique_size++;
                    if (i) {
                        goto error;
                    }
                }
            } else if (!strcmp(child->name, "default")) {
                GETVAL(ctx, value, child, "value");
                u = strlen(value);
                d->dflt[d->dflt_size++] = lydict_insert(ctx, value, u);

                if (dev_target->nodetype == LYS_CHOICE) {
                    choice = (struct lys_node_choice *)dev_target;
                    rc = resolve_choice_default_schema_nodeid(value, choice->child, (const struct lys_node **)&node);
                    if (rc || !node) {
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                        goto error;
                    }
                    if (d->mod == LY_DEVIATE_DEL) {
                        if (!choice->dflt || (choice->dflt != node)) {
                            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Value differs from the target being deleted.");
                            goto error;
                        }
                        choice->dflt = NULL;
                        /* remove extensions of this default instance from the target node */
                        j = -1;
                        while ((j = lys_ext_iter(dev_target->ext, dev_target->ext_size, j + 1, LYEXT_SUBSTMT_DEFAULT)) != -1) {
                            lyp_ext_instance_rm(ctx, &dev_target->ext, &dev_target->ext_size, j);
                            --j;
                        }
                    } else { /* add or replace */
                        choice->dflt = node;
                        if (!choice->dflt) {
                            /* default branch not found */
                            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                            goto error;
                        }
                    }
                } else if (dev_target->nodetype == LYS_LEAF) {
                    leaf = (struct lys_node_leaf *)dev_target;
                    if (d->mod == LY_DEVIATE_DEL) {
                        if (!leaf->dflt || !ly_strequal(leaf->dflt, value, 1)) {
                            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Value differs from the target being deleted.");
                            goto error;
                        }
                        /* remove value */
                        lydict_remove(ctx, leaf->dflt);
                        leaf->dflt = NULL;
                        leaf->flags &= ~LYS_DFLTJSON;

                        /* remove extensions of this default instance from the target node */
                        j = -1;
                        while ((j = lys_ext_iter(dev_target->ext, dev_target->ext_size, j + 1, LYEXT_SUBSTMT_DEFAULT)) != -1) {
                            lyp_ext_instance_rm(ctx, &dev_target->ext, &dev_target->ext_size, j);
                            --j;
                        }
                    } else { /* add (already checked) and replace */
                        /* remove value */
                        lydict_remove(ctx, leaf->dflt);
                        leaf->flags &= ~LYS_DFLTJSON;

                        /* set new value */
                        leaf->dflt = lydict_insert(ctx, value, u);

                        /* remember to check it later (it may not fit now, because the type can be deviated too) */
                        ly_set_add(dflt_check, dev_target, 0);
                    }
                } else { /* LYS_LEAFLIST */
                    llist = (struct lys_node_leaflist *)dev_target;
                    if (d->mod == LY_DEVIATE_DEL) {
                        /* find and remove the value in target list */
                        for (i = 0; i < llist->dflt_size; i++) {
                            if (llist->dflt[i] && ly_strequal(llist->dflt[i], value, 1)) {
                                /* match, remove the value */
                                lydict_remove(ctx, llist->dflt[i]);
                                llist->dflt[i] = NULL;

                                /* remove extensions of this default instance from the target node */
                                j = -1;
                                while ((j = lys_ext_iter(dev_target->ext, dev_target->ext_size, j + 1, LYEXT_SUBSTMT_DEFAULT)) != -1) {
                                    if (dev_target->ext[j]->insubstmt_index == i) {
                                        lyp_ext_instance_rm(ctx, &dev_target->ext, &dev_target->ext_size, j);
                                        --j;
                                    } else if (dev_target->ext[j]->insubstmt_index > i) {
                                        /* decrease the substatement index of the extension because of the changed array of defaults */
                                        dev_target->ext[j]->insubstmt_index--;
                                    }
                                }
                                break;
                            }
                        }
                        if (i == llist->dflt_size) {
                            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "The default value to delete not found in the target node.");
                            goto error;
                        }
                    } else {
                        /* add or replace, anyway we place items into the deviate's list
                           which propagates to the target */
                        /* we just want to check that the value isn't already in the list */
                        for (i = 0; i < llist->dflt_size; i++) {
                            if (ly_strequal(llist->dflt[i], value, 1)) {
                                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Duplicated default value \"%s\".", value);
                                goto error;
                            }
                        }
                        /* store it in target node */
                        llist->dflt[llist->dflt_size++] = lydict_insert(ctx, value, u);

                        /* remember to check it later (it may not fit now, but the type can be deviated too) */
                        ly_set_add(dflt_check, dev_target, 0);
                        llist->flags &= ~LYS_DFLTJSON;
                    }
                }
            }
        }

        lyp_reduce_ext_list(&d->ext, d->ext_size, c_ext2 + d->ext_size);

        if (c_dflt && dev_target->nodetype == LYS_LEAFLIST && d->mod == LY_DEVIATE_DEL) {
            /* consolidate the final list in the target after removing items from it */
            llist = (struct lys_node_leaflist *)dev_target;
            for (i = j = 0; j < llist->dflt_size; j++) {
                llist->dflt[i] = llist->dflt[j];
                if (llist->dflt[i]) {
                    i++;
                }
            }
            llist->dflt_size = i + 1;
        }
    }

    lyp_reduce_ext_list(&dev->ext, dev->ext_size, c_ext + dev->ext_size);

    /* now check whether default value, if any, matches the type */
    if (!(ctx->models.flags & LY_CTX_TRUSTED)) {
        for (u = 0; u < dflt_check->number; ++u) {
            value = NULL;
            rc = EXIT_SUCCESS;
            if (dflt_check->set.s[u]->nodetype == LYS_LEAF) {
                leaf = (struct lys_node_leaf *)dflt_check->set.s[u];
                value = leaf->dflt;
                rc = unres_schema_add_node(module, unres, &leaf->type, UNRES_TYPE_DFLT, (struct lys_node *)(&leaf->dflt));
            } else { /* LYS_LEAFLIST */
                llist = (struct lys_node_leaflist *)dflt_check->set.s[u];
                for (j = 0; j < llist->dflt_size; j++) {
                    rc = unres_schema_add_node(module, unres, &llist->type, UNRES_TYPE_DFLT,
                                            (struct lys_node *)(&llist->dflt[j]));
                    if (rc == -1) {
                        value = llist->dflt[j];
                        break;
                    }
                }

            }
            if (rc == -1) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL,
                    "The default value \"%s\" of the deviated node \"%s\" no longer matches its type.",
                    dev->target_name);
                goto error;
            }
        }
    }

    /* mark all the affected modules as deviated and implemented */
    for(parent = dev_target; parent; parent = lys_parent(parent)) {
        mod = lys_node_module(parent);
        if (module != mod) {
            mod->deviated = 1;            /* main module */
            parent->module->deviated = 1; /* possible submodule */
            if (!mod->implemented) {
                mod->implemented = 1;
                if (unres_schema_add_node(mod, unres, NULL, UNRES_MOD_IMPLEMENT, NULL) == -1) {
                    goto error;
                }
            }
        }
    }

    ly_set_free(dflt_check);
    return EXIT_SUCCESS;

error:
    ly_set_free(dflt_check);
    return EXIT_FAILURE;
}