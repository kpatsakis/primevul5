yin_parse_extcomplex_str(struct lys_module *mod, struct lyxml_elem *node,
                         struct lys_ext_instance_complex *ext, LY_STMT stmt,
                         int argelem, const char *argname, struct unres_schema *unres)
{
    int c;
    const char **str, ***p = NULL, *value;
    void *reallocated;
    struct lyext_substmt *info;

    str = lys_ext_complex_get_substmt(stmt, ext, &info);
    if (!str) {
        LOGVAL(mod->ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node->name, node->parent->name);
        return EXIT_FAILURE;
    }
    if (info->cardinality < LY_STMT_CARD_SOME && *str) {
        LOGVAL(mod->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, node->parent->name);
        return EXIT_FAILURE;
    }

    c = 0;
    if (info->cardinality >= LY_STMT_CARD_SOME) {
        /* there can be multiple instances, str is actually const char *** */
        p = (const char ***)str;
        if (!p[0]) {
            /* allocate initial array */
            p[0] = malloc(2 * sizeof(const char *));
            LY_CHECK_ERR_RETURN(!p[0], LOGMEM(mod->ctx), EXIT_FAILURE);
            if (stmt == LY_STMT_BELONGSTO) {
                /* allocate another array for the belongs-to's prefixes */
                p[1] = malloc(2 * sizeof(const char *));
                LY_CHECK_ERR_RETURN(!p[1], LOGMEM(mod->ctx), EXIT_FAILURE);
            } else if (stmt == LY_STMT_ARGUMENT) {
                /* allocate another array for the yin element */
                ((uint8_t **)p)[1] = malloc(2 * sizeof(uint8_t));
                LY_CHECK_ERR_RETURN(!p[1], LOGMEM(mod->ctx), EXIT_FAILURE);
            }
        } else {
            /* get the index in the array to add new item */
            for (c = 0; p[0][c]; c++);
        }
        str = p[0];
    }
    if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, (LYEXT_SUBSTMT)stmt, c, unres)) {
        return EXIT_FAILURE;
    }

    if (argelem) {
        str[c] = read_yin_subnode(mod->ctx, node, argname);
        if (!str[c]) {
            return EXIT_FAILURE;
        }
    } else {
        str[c] = lyxml_get_attr(node, argname, NULL);
        if (!str[c]) {
            LOGVAL(mod->ctx, LYE_MISSARG, LY_VLOG_NONE, NULL, argname, node->name);
            return EXIT_FAILURE;
        } else {
            str[c] = lydict_insert(mod->ctx, str[c], 0);
        }

        if (stmt == LY_STMT_BELONGSTO) {
            /* get the belongs-to's mandatory prefix substatement */
            if (!node->child) {
                LOGVAL(mod->ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "prefix", node->name);
                return EXIT_FAILURE;
            } else if (strcmp(node->child->name, "prefix")) {
                LOGVAL(mod->ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->child->name);
                return EXIT_FAILURE;
            } else if (node->child->next) {
                LOGVAL(mod->ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->child->next->name);
                return EXIT_FAILURE;
            }
            /* and now finally get the value */
            if (p) {
                str = p[1];
            } else {
                str++;
            }
            str[c] = lyxml_get_attr(node->child, "value", ((void *)0));
            if (!str[c]) {
                LOGVAL(mod->ctx, LYE_MISSARG, LY_VLOG_NONE, NULL, "value", node->child->name);
                return EXIT_FAILURE;
            }
            str[c] = lydict_insert(mod->ctx, str[c], 0);

            if (!str[c] || lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node->child, LYEXT_SUBSTMT_PREFIX, c, unres)) {
                return EXIT_FAILURE;
            }
        } else if (stmt == LY_STMT_ARGUMENT) {
            str = (p) ? p[1] : str + 1;
            if (!node->child) {
                /* default value of yin element */
                ((uint8_t *)str)[c] = 2;
            } else {
                /* get optional yin-element substatement */
                if (strcmp(node->child->name, "yin-element")) {
                    LOGVAL(mod->ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->child->name);
                    return EXIT_FAILURE;
                } else if (node->child->next) {
                    LOGVAL(mod->ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, node->child->next->name);
                    return EXIT_FAILURE;
                } else {
                    /* and now finally get the value */
                    value = lyxml_get_attr(node->child, "value", NULL);
                    if (!value) {
                        LOGVAL(mod->ctx, LYE_MISSARG, LY_VLOG_NONE, NULL, "value", node->child->name);
                        return EXIT_FAILURE;
                    }
                    if (ly_strequal(value, "true", 0)) {
                        ((uint8_t *)str)[c] = 1;
                    } else if (ly_strequal(value, "false", 0)) {
                        ((uint8_t *)str)[c] = 2;
                    } else {
                        LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, str, node->name);
                        return EXIT_FAILURE;
                    }

                    if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node->child, LYEXT_SUBSTMT_YINELEM, c, unres)) {
                        return EXIT_FAILURE;
                    }
                }
            }
        }
    }
    if (p) {
        /* enlarge the array(s) */
        reallocated = realloc(p[0], (c + 2) * sizeof(const char *));
        if (!reallocated) {
            LOGMEM(mod->ctx);
            lydict_remove(mod->ctx, p[0][c]);
            p[0][c] = NULL;
            return EXIT_FAILURE;
        }
        p[0] = reallocated;
        p[0][c + 1] = NULL;

        if (stmt == LY_STMT_BELONGSTO) {
            /* enlarge the second belongs-to's array with prefixes */
            reallocated = realloc(p[1], (c + 2) * sizeof(const char *));
            if (!reallocated) {
                LOGMEM(mod->ctx);
                lydict_remove(mod->ctx, p[1][c]);
                p[1][c] = NULL;
                return EXIT_FAILURE;
            }
            p[1] = reallocated;
            p[1][c + 1] = NULL;
        } else if (stmt == LY_STMT_ARGUMENT){
            /* enlarge the second argument's array with yin element */
            reallocated = realloc(p[1], (c + 2) * sizeof(uint8_t));
            if (!reallocated) {
                LOGMEM(mod->ctx);
                ((uint8_t *)p[1])[c] = 0;
                return EXIT_FAILURE;
            }
            p[1] = reallocated;
            ((uint8_t *)p[1])[c + 1] = 0;
        }
    }

    return EXIT_SUCCESS;
}