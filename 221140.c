fill_yin_type(struct lys_module *module, struct lys_node *parent, struct lyxml_elem *yin, struct lys_type *type,
              int parenttype, struct unres_schema *unres)
{
    const char *value, *name, *module_name = NULL;
    struct lys_node *siter;
    struct lyxml_elem *next, *next2, *node, *child, exts;
    struct lys_restr **restrs, *restr;
    struct lys_type_bit bit, *bits_sc = NULL;
    struct lys_type_enum *enms_sc = NULL; /* shortcut */
    struct lys_type *dertype;
    struct ly_ctx *ctx = module->ctx;
    int rc, val_set, c_ftrs, c_ext = 0;
    unsigned int i, j;
    int ret = -1;
    int64_t v, v_ = 0;
    int64_t p, p_;
    size_t len;
    int in_grp = 0;
    char *buf, modifier;

    /* init */
    memset(&exts, 0, sizeof exts);

    GETVAL(ctx, value, yin, "name");
    value = transform_schema2json(module, value);
    if (!value) {
        goto error;
    }

    i = parse_identifier(value);
    if (i < 1) {
        LOGVAL(ctx, LYE_INCHAR, LY_VLOG_NONE, NULL, value[-i], &value[-i]);
        lydict_remove(ctx, value);
        goto error;
    }
    /* module name */
    name = value;
    if (value[i]) {
        module_name = lydict_insert(ctx, value, i);
        name += i;
        if ((name[0] != ':') || (parse_identifier(name + 1) < 1)) {
            LOGVAL(ctx, LYE_INCHAR, LY_VLOG_NONE, NULL, name[0], name);
            lydict_remove(ctx, module_name);
            lydict_remove(ctx, value);
            goto error;
        }
        /* name is in dictionary, but moved */
        ++name;
    }

    rc = resolve_superior_type(name, module_name, module, parent, &type->der);
    if (rc == -1) {
        LOGVAL(ctx, LYE_INMOD, LY_VLOG_NONE, NULL, module_name);
        lydict_remove(ctx, module_name);
        lydict_remove(ctx, value);
        goto error;

    /* the type could not be resolved or it was resolved to an unresolved typedef */
    } else if (rc == EXIT_FAILURE) {
        LOGVAL(ctx, LYE_NORESOLV, LY_VLOG_NONE, NULL, "type", name);
        lydict_remove(ctx, module_name);
        lydict_remove(ctx, value);
        ret = EXIT_FAILURE;
        goto error;
    }
    lydict_remove(ctx, module_name);
    lydict_remove(ctx, value);

    if (type->value_flags & LY_VALUE_UNRESGRP) {
        /* resolved type in grouping, decrease the grouping's nacm number to indicate that one less
         * unresolved item left inside the grouping, LYTYPE_GRP used as a flag for types inside a grouping. */
        for (siter = parent; siter && (siter->nodetype != LYS_GROUPING); siter = lys_parent(siter));
        if (siter) {
            assert(((struct lys_node_grp *)siter)->unres_count);
            ((struct lys_node_grp *)siter)->unres_count--;
        } else {
            LOGINT(ctx);
            goto error;
        }
        type->value_flags &= ~LY_VALUE_UNRESGRP;
    }
    type->base = type->der->type.base;

    /* check status */
    if (lyp_check_status(type->parent->flags, type->parent->module, type->parent->name,
                         type->der->flags, type->der->module, type->der->name,  parent)) {
        return -1;
    }

    /* parse extension instances */
    LY_TREE_FOR_SAFE(yin->child, next, node) {
        if (!node->ns) {
            /* garbage */
            lyxml_free(ctx, node);
            continue;
        } else if (!strcmp(node->ns->value, LY_NSYIN)) {
            /* YANG (YIN) statements - process later */
            continue;
        }

        YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ext, type->ext_size, "extensions", "type", error);

        lyxml_unlink_elem(ctx, node, 2);
        lyxml_add_child(ctx, &exts, node);
        c_ext++;
    }
    if (c_ext) {
        type->ext = calloc(c_ext, sizeof *type->ext);
        LY_CHECK_ERR_GOTO(!type->ext, LOGMEM(ctx), error);

        LY_TREE_FOR_SAFE(exts.child, next, node) {
            rc = lyp_yin_fill_ext(type, LYEXT_PAR_TYPE, 0, 0, module, node, &type->ext, &type->ext_size, unres);
            if (rc) {
                goto error;
            }
        }

        lyp_reduce_ext_list(&type->ext, type->ext_size, c_ext + type->ext_size);
    }

    switch (type->base) {
    case LY_TYPE_BITS:
        /* RFC 6020 9.7.4 - bit */

        /* get bit specifications, at least one must be present */
        LY_TREE_FOR_SAFE(yin->child, next, node) {
            if (!strcmp(node->name, "bit")) {
                YIN_CHECK_ARRAY_OVERFLOW_CODE(ctx, type->info.bits.count, type->info.bits.count, "bits", "type",
                                              type->info.bits.count = 0; goto error);
                type->info.bits.count++;
            } else {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                type->info.bits.count = 0;
                goto error;
            }
        }
        dertype = &type->der->type;
        if (!dertype->der) {
            if (!type->info.bits.count) {
                /* type is derived directly from buit-in bits type and bit statement is required */
                LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "bit", "type");
                goto error;
            }
        } else {
            for (; !dertype->info.enums.count; dertype = &dertype->der->type);
            if (module->version < 2 && type->info.bits.count) {
                /* type is not directly derived from buit-in bits type and bit statement is prohibited,
                 * since YANG 1.1 the bit statements can be used to restrict the base bits type */
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "bit");
                type->info.bits.count = 0;
                goto error;
            }
        }

        if (type->info.bits.count) {
            type->info.bits.bit = calloc(type->info.bits.count, sizeof *type->info.bits.bit);
            LY_CHECK_ERR_GOTO(!type->info.bits.bit, LOGMEM(ctx), error);
        }

        p = 0;
        i = 0;
        LY_TREE_FOR(yin->child, next) {
            c_ftrs = 0;

            GETVAL(ctx, value, next, "name");
            if (lyp_check_identifier(ctx, value, LY_IDENT_SIMPLE, NULL, NULL)) {
                goto error;
            }

            type->info.bits.bit[i].name = lydict_insert(ctx, value, strlen(value));
            if (read_yin_common(module, NULL, &type->info.bits.bit[i], LYEXT_PAR_TYPE_BIT, next, 0, unres)) {
                type->info.bits.count = i + 1;
                goto error;
            }

            if (!dertype->der) { /* directly derived type from bits built-in type */
                /* check the name uniqueness */
                for (j = 0; j < i; j++) {
                    if (!strcmp(type->info.bits.bit[j].name, type->info.bits.bit[i].name)) {
                        LOGVAL(ctx, LYE_BITS_DUPNAME, LY_VLOG_NONE, NULL, type->info.bits.bit[i].name);
                        type->info.bits.count = i + 1;
                        goto error;
                    }
                }
            } else {
                /* restricted bits type - the name MUST be used in the base type */
                bits_sc = dertype->info.bits.bit;
                for (j = 0; j < dertype->info.bits.count; j++) {
                    if (ly_strequal(bits_sc[j].name, value, 1)) {
                        break;
                    }
                }
                if (j == dertype->info.bits.count) {
                    LOGVAL(ctx, LYE_BITS_INNAME, LY_VLOG_NONE, NULL, value);
                    type->info.bits.count = i + 1;
                    goto error;
                }
            }


            p_ = -1;
            LY_TREE_FOR_SAFE(next->child, next2, node) {
                if (!node->ns) {
                    /* garbage */
                    continue;
                } else if (strcmp(node->ns->value, LY_NSYIN)) {
                    /* extension */
                    if (lyp_yin_parse_subnode_ext(module, &type->info.bits.bit[i], LYEXT_PAR_TYPE_BIT, node,
                                                  LYEXT_SUBSTMT_SELF, 0, unres)) {
                        goto error;
                    }
                } else if (!strcmp(node->name, "position")) {
                    if (p_ != -1) {
                        LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, next->name);
                        type->info.bits.count = i + 1;
                        goto error;
                    }

                    GETVAL(ctx, value, node, "value");
                    p_ = strtoll(value, NULL, 10);

                    /* range check */
                    if (p_ < 0 || p_ > UINT32_MAX) {
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "bit/position");
                        type->info.bits.count = i + 1;
                        goto error;
                    }
                    type->info.bits.bit[i].pos = (uint32_t)p_;

                    if (!dertype->der) { /* directly derived type from bits built-in type */
                        /* keep the highest enum value for automatic increment */
                        if (type->info.bits.bit[i].pos >= p) {
                            p = type->info.bits.bit[i].pos;
                            p++;
                        } else {
                            /* check that the value is unique */
                            for (j = 0; j < i; j++) {
                                if (type->info.bits.bit[j].pos == type->info.bits.bit[i].pos) {
                                    LOGVAL(ctx, LYE_BITS_DUPVAL, LY_VLOG_NONE, NULL,
                                           type->info.bits.bit[i].pos, type->info.bits.bit[i].name,
                                           type->info.bits.bit[j].name);
                                    type->info.bits.count = i + 1;
                                    goto error;
                                }
                            }
                        }
                    }

                    if (lyp_yin_parse_subnode_ext(module, &type->info.bits.bit[i], LYEXT_PAR_TYPE_BIT, node,
                                             LYEXT_SUBSTMT_POSITION, 0, unres)) {
                        goto error;
                    }

                    for (j = 0; j < type->info.bits.bit[i].ext_size; ++j) {
                        /* set flag, which represent LYEXT_OPT_VALID */
                        if (type->info.bits.bit[i].ext[j]->flags & LYEXT_OPT_VALID) {
                            type->parent->flags |= LYS_VALID_EXT;
                            break;
                        }
                    }

                } else if ((module->version >= 2) && !strcmp(node->name, "if-feature")) {
                    YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ftrs, type->info.bits.bit[i].iffeature_size, "if-features", "bit", error);
                    c_ftrs++;
                } else {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                    goto error;
                }
            }

            if (!dertype->der) { /* directly derived type from bits built-in type */
                if (p_ == -1) {
                    /* assign value automatically */
                    if (p > UINT32_MAX) {
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, "4294967295", "bit/position");
                        type->info.bits.count = i + 1;
                        goto error;
                    }
                    type->info.bits.bit[i].pos = (uint32_t)p;
                    type->info.bits.bit[i].flags |= LYS_AUTOASSIGNED;
                    p++;
                }
            } else { /* restricted bits type */
                if (p_ == -1) {
                    /* automatically assign position from base type */
                    type->info.bits.bit[i].pos = bits_sc[j].pos;
                    type->info.bits.bit[i].flags |= LYS_AUTOASSIGNED;
                } else {
                    /* check that the assigned position corresponds to the original
                     * position of the bit in the base type */
                    if (p_ != bits_sc[j].pos) {
                        /* p_ - assigned position in restricted bits
                         * bits_sc[j].pos - position assigned to the corresponding bit (detected above) in base type */
                        LOGVAL(ctx, LYE_BITS_INVAL, LY_VLOG_NONE, NULL, type->info.bits.bit[i].pos,
                               type->info.bits.bit[i].name, bits_sc[j].pos);
                        type->info.bits.count = i + 1;
                        goto error;
                    }
                }
            }

            /* if-features */
            if (c_ftrs) {
                bits_sc = &type->info.bits.bit[i];
                bits_sc->iffeature = calloc(c_ftrs, sizeof *bits_sc->iffeature);
                if (!bits_sc->iffeature) {
                    LOGMEM(ctx);
                    type->info.bits.count = i + 1;
                    goto error;
                }

                LY_TREE_FOR(next->child, node) {
                    if (!strcmp(node->name, "if-feature")) {
                        rc = fill_yin_iffeature((struct lys_node *)type->parent, 0, node,
                                                &bits_sc->iffeature[bits_sc->iffeature_size], unres);
                        bits_sc->iffeature_size++;
                        if (rc) {
                            type->info.bits.count = i + 1;
                            goto error;
                        }
                    }
                }
            }

            /* keep them ordered by position */
            j = i;
            while (j && type->info.bits.bit[j - 1].pos > type->info.bits.bit[j].pos) {
                /* switch them */
                memcpy(&bit, &type->info.bits.bit[j], sizeof bit);
                memcpy(&type->info.bits.bit[j], &type->info.bits.bit[j - 1], sizeof bit);
                memcpy(&type->info.bits.bit[j - 1], &bit, sizeof bit);
                j--;
            }

            ++i;
        }
        break;

    case LY_TYPE_DEC64:
        /* RFC 6020 9.2.4 - range and 9.3.4 - fraction-digits */
        LY_TREE_FOR(yin->child, node) {

            if (!strcmp(node->name, "range")) {
                if (type->info.dec64.range) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                    goto error;
                }

                GETVAL(ctx, value, node, "value");
                type->info.dec64.range = calloc(1, sizeof *type->info.dec64.range);
                LY_CHECK_ERR_GOTO(!type->info.dec64.range, LOGMEM(ctx), error);
                type->info.dec64.range->expr = lydict_insert(ctx, value, 0);

                /* get possible substatements */
                if (read_restr_substmt(module, type->info.dec64.range, node, unres)) {
                    goto error;
                }
                for (j = 0; j < type->info.dec64.range->ext_size; ++j) {
                    /* set flag, which represent LYEXT_OPT_VALID */
                    if (type->info.dec64.range->ext[j]->flags & LYEXT_OPT_VALID) {
                        type->parent->flags |= LYS_VALID_EXT;
                        break;
                    }
                }
            } else if (!strcmp(node->name, "fraction-digits")) {
                if (type->info.dec64.dig) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                    goto error;
                }
                GETVAL(ctx, value, node, "value");
                v = strtol(value, NULL, 10);

                /* range check */
                if (v < 1 || v > 18) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, node->name);
                    goto error;
                }
                type->info.dec64.dig = (uint8_t)v;
                type->info.dec64.div = 10;
                for (i = 1; i < v; i++) {
                    type->info.dec64.div *= 10;
                }

                /* extensions */
                if (lyp_yin_parse_subnode_ext(module, type, LYEXT_PAR_TYPE, node, LYEXT_SUBSTMT_DIGITS, 0, unres)) {
                    goto error;
                }
            } else {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                goto error;
            }
        }

        /* mandatory sub-statement(s) check */
        if (!type->info.dec64.dig && !type->der->type.der) {
            /* decimal64 type directly derived from built-in type requires fraction-digits */
            LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "fraction-digits", "type");
            goto error;
        }
        if (type->info.dec64.dig && type->der->type.der) {
            /* type is not directly derived from buit-in type and fraction-digits statement is prohibited */
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "fraction-digits");
            goto error;
        }

        /* copy fraction-digits specification from parent type for easier internal use */
        if (type->der->type.der) {
            type->info.dec64.dig = type->der->type.info.dec64.dig;
            type->info.dec64.div = type->der->type.info.dec64.div;
        }

        if (type->info.dec64.range && lyp_check_length_range(ctx, type->info.dec64.range->expr, type)) {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "range");
            goto error;
        }
        break;

    case LY_TYPE_ENUM:
        /* RFC 6020 9.6 - enum */

        /* get enum specifications, at least one must be present */
        LY_TREE_FOR_SAFE(yin->child, next, node) {

            if (!strcmp(node->name, "enum")) {
                YIN_CHECK_ARRAY_OVERFLOW_CODE(ctx, type->info.enums.count, type->info.enums.count, "enums", "type",
                                              type->info.enums.count = 0; goto error);
                type->info.enums.count++;
            } else {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                type->info.enums.count = 0;
                goto error;
            }
        }
        dertype = &type->der->type;
        if (!dertype->der) {
            if (!type->info.enums.count) {
                /* type is derived directly from buit-in enumeartion type and enum statement is required */
                LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "enum", "type");
                goto error;
            }
        } else {
            for (; !dertype->info.enums.count; dertype = &dertype->der->type);
            if (module->version < 2 && type->info.enums.count) {
                /* type is not directly derived from built-in enumeration type and enum statement is prohibited
                 * in YANG 1.0, since YANG 1.1 enum statements can be used to restrict the base enumeration type */
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "enum");
                type->info.enums.count = 0;
                goto error;
            }
        }

        if (type->info.enums.count) {
            type->info.enums.enm = calloc(type->info.enums.count, sizeof *type->info.enums.enm);
            LY_CHECK_ERR_GOTO(!type->info.enums.enm, LOGMEM(ctx), error);
        }

        v = 0;
        i = 0;
        LY_TREE_FOR(yin->child, next) {
            c_ftrs = 0;

            GETVAL(ctx, value, next, "name");
            if (!value[0]) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "enum name");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Enum name must not be empty.");
                goto error;
            }
            type->info.enums.enm[i].name = lydict_insert(ctx, value, strlen(value));
            if (read_yin_common(module, NULL, &type->info.enums.enm[i], LYEXT_PAR_TYPE_ENUM, next, 0, unres)) {
                type->info.enums.count = i + 1;
                goto error;
            }

            /* the assigned name MUST NOT have any leading or trailing whitespace characters */
            value = type->info.enums.enm[i].name;
            if (isspace(value[0]) || isspace(value[strlen(value) - 1])) {
                LOGVAL(ctx, LYE_ENUM_WS, LY_VLOG_NONE, NULL, value);
                type->info.enums.count = i + 1;
                goto error;
            }

            if (!dertype->der) { /* directly derived type from enumeration built-in type */
                /* check the name uniqueness */
                for (j = 0; j < i; j++) {
                    if (ly_strequal(type->info.enums.enm[j].name, value, 1)) {
                        LOGVAL(ctx, LYE_ENUM_DUPNAME, LY_VLOG_NONE, NULL, value);
                        type->info.enums.count = i + 1;
                        goto error;
                    }
                }
            } else {
                /* restricted enumeration type - the name MUST be used in the base type */
                enms_sc = dertype->info.enums.enm;
                for (j = 0; j < dertype->info.enums.count; j++) {
                    if (ly_strequal(enms_sc[j].name, value, 1)) {
                        break;
                    }
                }
                if (j == dertype->info.enums.count) {
                    LOGVAL(ctx, LYE_ENUM_INNAME, LY_VLOG_NONE, NULL, value);
                    type->info.enums.count = i + 1;
                    goto error;
                }
            }

            val_set = 0;
            LY_TREE_FOR_SAFE(next->child, next2, node) {
                if (!node->ns) {
                    /* garbage */
                    continue;
                } else if (strcmp(node->ns->value, LY_NSYIN)) {
                    /* extensions */
                    if (lyp_yin_parse_subnode_ext(module, &type->info.enums.enm[i], LYEXT_PAR_TYPE_ENUM, node,
                                             LYEXT_SUBSTMT_SELF, 0, unres)) {
                        goto error;
                    }
                } else if (!strcmp(node->name, "value")) {
                    if (val_set) {
                        LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, next->name);
                        type->info.enums.count = i + 1;
                        goto error;
                    }

                    GETVAL(ctx, value, node, "value");
                    v_ = strtoll(value, NULL, 10);

                    /* range check */
                    if (v_ < INT32_MIN || v_ > INT32_MAX) {
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "enum/value");
                        type->info.enums.count = i + 1;
                        goto error;
                    }
                    type->info.enums.enm[i].value = v_;

                    if (!dertype->der) { /* directly derived type from enumeration built-in type */
                        if (!i) {
                            /* change value, which is assigned automatically, if first enum has value. */
                            v = type->info.enums.enm[i].value;
                            v++;
                        } else {
                            /* keep the highest enum value for automatic increment */
                            if (type->info.enums.enm[i].value >= v) {
                                v = type->info.enums.enm[i].value;
                                v++;
                            } else {
                                /* check that the value is unique */
                                for (j = 0; j < i; j++) {
                                    if (type->info.enums.enm[j].value == type->info.enums.enm[i].value) {
                                        LOGVAL(ctx, LYE_ENUM_DUPVAL, LY_VLOG_NONE, NULL,
                                               type->info.enums.enm[i].value, type->info.enums.enm[i].name,
                                               type->info.enums.enm[j].name);
                                        type->info.enums.count = i + 1;
                                        goto error;
                                    }
                                }
                            }
                        }
                    }
                    val_set = 1;

                    if (lyp_yin_parse_subnode_ext(module, &type->info.enums.enm[i], LYEXT_PAR_TYPE_ENUM, node,
                                             LYEXT_SUBSTMT_VALUE, 0, unres)) {
                        goto error;
                    }

                    for (j = 0; j < type->info.enums.enm[i].ext_size; ++j) {
                        /* set flag, which represent LYEXT_OPT_VALID */
                        if (type->info.enums.enm[i].ext[j]->flags & LYEXT_OPT_VALID) {
                            type->parent->flags |= LYS_VALID_EXT;
                            break;
                        }
                    }
                } else if ((module->version >= 2) && !strcmp(node->name, "if-feature")) {
                    YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, c_ftrs, type->info.enums.enm[i].iffeature_size, "if-features", "enum", error);
                    c_ftrs++;

                } else {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                    goto error;
                }
            }

            if (!dertype->der) { /* directly derived type from enumeration */
                if (!val_set) {
                    /* assign value automatically */
                    if (v > INT32_MAX) {
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, "2147483648", "enum/value");
                        type->info.enums.count = i + 1;
                        goto error;
                    }
                    type->info.enums.enm[i].value = v;
                    type->info.enums.enm[i].flags |= LYS_AUTOASSIGNED;
                    v++;
                }
            } else { /* restricted enum type */
                if (!val_set) {
                    /* automatically assign value from base type */
                    type->info.enums.enm[i].value = enms_sc[j].value;
                    type->info.enums.enm[i].flags |= LYS_AUTOASSIGNED;
                } else {
                    /* check that the assigned value corresponds to the original
                     * value of the enum in the base type */
                    if (v_ != enms_sc[j].value) {
                        /* v_ - assigned value in restricted enum
                         * enms_sc[j].value - value assigned to the corresponding enum (detected above) in base type */
                        LOGVAL(ctx, LYE_ENUM_INVAL, LY_VLOG_NONE, NULL,
                               type->info.enums.enm[i].value, type->info.enums.enm[i].name, enms_sc[j].value);
                        type->info.enums.count = i + 1;
                        goto error;
                    }
                }
            }

            /* if-features */
            if (c_ftrs) {
                enms_sc = &type->info.enums.enm[i];
                enms_sc->iffeature = calloc(c_ftrs, sizeof *enms_sc->iffeature);
                if (!enms_sc->iffeature) {
                    LOGMEM(ctx);
                    type->info.enums.count = i + 1;
                    goto error;
                }

                LY_TREE_FOR(next->child, node) {
                    if (!strcmp(node->name, "if-feature")) {
                        rc = fill_yin_iffeature((struct lys_node *)type->parent, 0, node,
                                                &enms_sc->iffeature[enms_sc->iffeature_size], unres);
                        enms_sc->iffeature_size++;
                        if (rc) {
                            type->info.enums.count = i + 1;
                            goto error;
                        }
                    }
                }
            }

            ++i;
        }
        break;

    case LY_TYPE_IDENT:
        /* RFC 6020 9.10 - base */

        /* get base specification, at least one must be present */
        LY_TREE_FOR_SAFE(yin->child, next, node) {

            if (strcmp(node->name, "base")) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                goto error;
            }

            GETVAL(ctx, value, yin->child, "name");
            /* store in the JSON format */
            value = transform_schema2json(module, value);
            if (!value) {
                goto error;
            }
            rc = unres_schema_add_str(module, unres, type, UNRES_TYPE_IDENTREF, value);
            lydict_remove(ctx, value);
            if (rc == -1) {
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(module, type, LYEXT_PAR_TYPE, node, LYEXT_SUBSTMT_BASE, 0, unres)) {
                goto error;
            }
        }

        if (!yin->child) {
            if (type->der->type.der) {
                /* this is just a derived type with no base required */
                break;
            }
            LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "base", "type");
            goto error;
        } else {
            if (type->der->type.der) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "base");
                goto error;
            }
        }
        if (yin->child->next) {
            LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, yin->child->next->name, yin->name);
            goto error;
        }
        break;

    case LY_TYPE_INST:
        /* RFC 6020 9.13.2 - require-instance */
        LY_TREE_FOR(yin->child, node) {
            if (!strcmp(node->name, "require-instance")) {
                if (type->info.inst.req) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                    goto error;
                }
                GETVAL(ctx, value, node, "value");
                if (!strcmp(value, "true")) {
                    type->info.inst.req = 1;
                } else if (!strcmp(value, "false")) {
                    type->info.inst.req = -1;
                } else {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, node->name);
                    goto error;
                }

                /* extensions */
                if (lyp_yin_parse_subnode_ext(module, type, LYEXT_PAR_TYPE, node, LYEXT_SUBSTMT_REQINSTANCE, 0, unres)) {
                    goto error;
                }
            } else {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                goto error;
            }
        }

        if (type->der->type.der && !type->info.inst.req) {
            /* inherit require-instance property */
            type->info.inst.req = type->der->type.info.inst.req;
        }

        break;

    case LY_TYPE_BINARY:
        /* RFC 6020 9.8.1, 9.4.4 - length, number of octets it contains */
    case LY_TYPE_INT8:
    case LY_TYPE_INT16:
    case LY_TYPE_INT32:
    case LY_TYPE_INT64:
    case LY_TYPE_UINT8:
    case LY_TYPE_UINT16:
    case LY_TYPE_UINT32:
    case LY_TYPE_UINT64:
        /* RFC 6020 9.2.4 - range */

        /* length and range are actually the same restriction, so process
         * them by this common code, we just need to differ the name and
         * structure where the information will be stored
         */
        if (type->base == LY_TYPE_BINARY) {
            restrs = &type->info.binary.length;
            name = "length";
        } else {
            restrs = &type->info.num.range;
            name = "range";
        }

        LY_TREE_FOR(yin->child, node) {

            if (!strcmp(node->name, name)) {
                if (*restrs) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                    goto error;
                }

                GETVAL(ctx, value, node, "value");
                if (lyp_check_length_range(ctx, value, type)) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, name);
                    goto error;
                }
                *restrs = calloc(1, sizeof **restrs);
                LY_CHECK_ERR_GOTO(!(*restrs), LOGMEM(ctx), error);
                (*restrs)->expr = lydict_insert(ctx, value, 0);

                /* get possible substatements */
                if (read_restr_substmt(module, *restrs, node, unres)) {
                    goto error;
                }

                for (j = 0; j < (*restrs)->ext_size; ++j) {
                    /* set flag, which represent LYEXT_OPT_VALID */
                    if ((*restrs)->ext[j]->flags & LYEXT_OPT_VALID) {
                        type->parent->flags |= LYS_VALID_EXT;
                        break;
                    }
                }
            } else {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                goto error;
            }
        }
        break;

    case LY_TYPE_LEAFREF:
        /* flag resolving for later use */
        if (!parenttype && lys_ingrouping(parent)) {
            /* just a flag - do not resolve */
            parenttype = 1;
        }

        /* RFC 6020 9.9.2 - path */
        LY_TREE_FOR(yin->child, node) {
            if (!strcmp(node->name, "path") && !type->der->type.der) {
                /* keep path for later */
            } else if (module->version >= 2 && !strcmp(node->name, "require-instance")) {
                if (type->info.lref.req) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                    goto error;
                }
                GETVAL(ctx, value, node, "value");
                if (!strcmp(value, "true")) {
                    type->info.lref.req = 1;
                } else if (!strcmp(value, "false")) {
                    type->info.lref.req = -1;
                } else {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, node->name);
                    goto error;
                }

                /* extensions */
                if (lyp_yin_parse_subnode_ext(module, type, LYEXT_PAR_TYPE, node, LYEXT_SUBSTMT_REQINSTANCE, 0, unres)) {
                    goto error;
                }
            } else {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                goto error;
            }
        }

        /* now that require-instance is properly set, try to find and resolve path */
        LY_TREE_FOR(yin->child, node) {
            if (!strcmp(node->name, "path") && !type->der->type.der) {
                if (type->info.lref.path) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                    goto error;
                }

                GETVAL(ctx, value, node, "value");
                /* store in the JSON format */
                type->info.lref.path = transform_schema2json(module, value);
                if (!type->info.lref.path) {
                    goto error;
                }

                /* try to resolve leafref path only when this is instantiated
                 * leaf, so it is not:
                 * - typedef's type,
                 * - in  grouping definition,
                 * - just instantiated in a grouping definition,
                 * because in those cases the nodes referenced in path might not be present
                 * and it is not a bug.  */
                if (!parenttype && unres_schema_add_node(module, unres, type, UNRES_TYPE_LEAFREF, parent) == -1) {
                    goto error;
                }

                /* extensions */
                if (lyp_yin_parse_subnode_ext(module, type, LYEXT_PAR_TYPE, node, LYEXT_SUBSTMT_PATH, 0, unres)) {
                    goto error;
                }

                break;
            }
        }

        if (!type->info.lref.path) {
            if (!type->der->type.der) {
                LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "path", "type");
                goto error;
            } else {
                /* copy leafref definition into the derived type */
                type->info.lref.path = lydict_insert(ctx, type->der->type.info.lref.path, 0);
                if (!type->info.lref.req) {
                    type->info.lref.req = type->der->type.info.lref.req;
                }
                /* and resolve the path at the place we are (if not in grouping/typedef) */
                if (!parenttype && unres_schema_add_node(module, unres, type, UNRES_TYPE_LEAFREF, parent) == -1) {
                    goto error;
                }
            }
        }

        break;

    case LY_TYPE_STRING:
        /* RFC 6020 9.4.4 - length */
        /* RFC 6020 9.4.6 - pattern */
        i = 0;
        LY_TREE_FOR_SAFE(yin->child, next, node) {

            if (!strcmp(node->name, "length")) {
                if (type->info.str.length) {
                    LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                    goto error;
                }

                GETVAL(ctx, value, node, "value");
                if (lyp_check_length_range(ctx, value, type)) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "length");
                    goto error;
                }
                type->info.str.length = calloc(1, sizeof *type->info.str.length);
                LY_CHECK_ERR_GOTO(!type->info.str.length, LOGMEM(ctx), error);
                type->info.str.length->expr = lydict_insert(ctx, value, 0);

                /* get possible sub-statements */
                if (read_restr_substmt(module, type->info.str.length, node, unres)) {
                    goto error;
                }

                for (j = 0; j < type->info.str.length->ext_size; ++j) {
                    /* set flag, which represent LYEXT_OPT_VALID */
                    if (type->info.str.length->ext[j]->flags & LYEXT_OPT_VALID) {
                        type->parent->flags |= LYS_VALID_EXT;
                        break;
                    }
                }
                lyxml_free(ctx, node);
            } else if (!strcmp(node->name, "pattern")) {
                YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, i, type->info.str.pat_count, "patterns", "type", error);
                i++;
            } else {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                goto error;
            }
        }
        /* store patterns in array */
        if (i) {
            if (!parenttype && parent && lys_ingrouping(parent)) {
                in_grp = 1;
            }
            type->info.str.patterns = calloc(i, sizeof *type->info.str.patterns);
            LY_CHECK_ERR_GOTO(!type->info.str.patterns, LOGMEM(ctx), error);
#ifdef LY_ENABLED_CACHE
            if (!in_grp) {
                /* do not compile patterns in groupings */
                type->info.str.patterns_pcre = calloc(2 * i, sizeof *type->info.str.patterns_pcre);
                LY_CHECK_ERR_GOTO(!type->info.str.patterns_pcre, LOGMEM(ctx), error);
            }
#endif
            LY_TREE_FOR(yin->child, node) {
                GETVAL(ctx, value, node, "value");

                if (in_grp) {
                    /* in grouping, just check the pattern syntax */
                    if (!(ctx->models.flags & LY_CTX_TRUSTED) && lyp_check_pattern(ctx, value, NULL)) {
                        goto error;
                    }
                }
#ifdef LY_ENABLED_CACHE
                else {
                    /* outside grouping, check syntax and precompile pattern for later use by libpcre */
                    if (lyp_precompile_pattern(ctx, value,
                            (pcre **)&type->info.str.patterns_pcre[type->info.str.pat_count * 2],
                            (pcre_extra **)&type->info.str.patterns_pcre[type->info.str.pat_count * 2 + 1])) {
                        goto error;
                    }
                }
#endif
                restr = &type->info.str.patterns[type->info.str.pat_count]; /* shortcut */
                type->info.str.pat_count++;

                modifier = 0x06; /* ACK */
                name = NULL;
                if (module->version >= 2) {
                    LY_TREE_FOR_SAFE(node->child, next2, child) {
                        if (child->ns && !strcmp(child->ns->value, LY_NSYIN) && !strcmp(child->name, "modifier")) {
                            if (name) {
                                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "modifier", node->name);
                                goto error;
                            }

                            GETVAL(ctx, name, child, "value");
                            if (!strcmp(name, "invert-match")) {
                                modifier = 0x15; /* NACK */
                            } else {
                                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, name, "modifier");
                                goto error;
                            }
                            /* get extensions of the modifier */
                            if (lyp_yin_parse_subnode_ext(module, restr, LYEXT_PAR_RESTR, child,
                                                          LYEXT_SUBSTMT_MODIFIER, 0, unres)) {
                                goto error;
                            }

                            lyxml_free(ctx, child);
                        }
                    }
                }

                len = strlen(value);
                buf = malloc((len + 2) * sizeof *buf); /* modifier byte + value + terminating NULL byte */
                LY_CHECK_ERR_GOTO(!buf, LOGMEM(ctx), error);
                buf[0] = modifier;
                strcpy(&buf[1], value);

                restr->expr = lydict_insert_zc(ctx, buf);

                /* get possible sub-statements */
                if (read_restr_substmt(module, restr, node, unres)) {
                    goto error;
                }

                for (j = 0; j < restr->ext_size; ++j) {
                    /* set flag, which represent LYEXT_OPT_VALID */
                    if (restr->ext[j]->flags & LYEXT_OPT_VALID) {
                        type->parent->flags |= LYS_VALID_EXT;
                        break;
                    }
                }
            }
        }
        break;

    case LY_TYPE_UNION:
        /* RFC 6020 7.4 - type */
        /* count number of types in union */
        i = 0;
        LY_TREE_FOR_SAFE(yin->child, next, node) {

            if (!strcmp(node->name, "type")) {
                if (type->der->type.der) {
                    /* type can be a substatement only in "union" type, not in derived types */
                    LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "type", "derived type");
                    goto error;
                }
                YIN_CHECK_ARRAY_OVERFLOW_GOTO(ctx, i, type->info.uni.count, "types", "type", error);
                i++;
            } else {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->name);
                goto error;
            }
        }

        if (!i && !type->der->type.der) {
            LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "type", "(union) type");
            goto error;
        }

        /* inherit instid presence information */
        if ((type->der->type.base == LY_TYPE_UNION) && type->der->type.info.uni.has_ptr_type) {
            type->info.uni.has_ptr_type = 1;
        }

        /* allocate array for union's types ... */
        if (i) {
            type->info.uni.types = calloc(i, sizeof *type->info.uni.types);
            LY_CHECK_ERR_GOTO(!type->info.uni.types, LOGMEM(ctx), error);
        }

        /* ... and fill the structures */
        LY_TREE_FOR(yin->child, node) {
            type->info.uni.types[type->info.uni.count].parent = type->parent;
            rc = fill_yin_type(module, parent, node, &type->info.uni.types[type->info.uni.count], parenttype, unres);
            if (!rc) {
                type->info.uni.count++;

                if (module->version < 2) {
                    /* union's type cannot be empty or leafref */
                    if (type->info.uni.types[type->info.uni.count - 1].base == LY_TYPE_EMPTY) {
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, "empty", node->name);
                        rc = -1;
                    } else if (type->info.uni.types[type->info.uni.count - 1].base == LY_TYPE_LEAFREF) {
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, "leafref", node->name);
                        rc = -1;
                    }
                }

                if ((type->info.uni.types[type->info.uni.count - 1].base == LY_TYPE_INST)
                        || (type->info.uni.types[type->info.uni.count - 1].base == LY_TYPE_LEAFREF)
                        || ((type->info.uni.types[type->info.uni.count - 1].base == LY_TYPE_UNION)
                        && type->info.uni.types[type->info.uni.count - 1].info.uni.has_ptr_type)) {
                    type->info.uni.has_ptr_type = 1;
                }
            }
            if (rc) {
                /* even if we got EXIT_FAILURE, throw it all away, too much trouble doing something else */
                for (i = 0; i < type->info.uni.count; ++i) {
                    lys_type_free(ctx, &type->info.uni.types[i], NULL);
                }
                free(type->info.uni.types);
                type->info.uni.types = NULL;
                type->info.uni.count = 0;
                type->info.uni.has_ptr_type = 0;
                type->der = NULL;
                type->base = LY_TYPE_DER;

                if (rc == EXIT_FAILURE) {
                    ret = EXIT_FAILURE;
                }
                goto error;
            }
        }
        break;

    case LY_TYPE_BOOL:
    case LY_TYPE_EMPTY:
        /* no sub-statement allowed */
        if (yin->child) {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, yin->child->name);
            goto error;
        }
        break;

    default:
        LOGINT(ctx);
        goto error;
    }

    for(j = 0; j < type->ext_size; ++j) {
        /* set flag, which represent LYEXT_OPT_VALID */
        if (type->ext[j]->flags & LYEXT_OPT_VALID) {
            type->parent->flags |= LYS_VALID_EXT;
            break;
        }
    }

    /* if derived type has extension, which need validate data */
    dertype = &type->der->type;
    while (dertype->der) {
        if (dertype->parent->flags & LYS_VALID_EXT) {
            type->parent->flags |= LYS_VALID_EXT;
        }
        dertype = &dertype->der->type;
    }

    return EXIT_SUCCESS;

error:
    lyxml_free_withsiblings(ctx, exts.child);
    return ret;
}