lyp_yin_parse_complex_ext(struct lys_module *mod, struct lys_ext_instance_complex *ext, struct lyxml_elem *yin,
                          struct unres_schema *unres)
{
    struct lyxml_elem *next, *node, *child;
    struct lys_type **type;
    void **pp, *p, *reallocated;
    const char *value, *name;
    char *endptr, modifier;
    struct lyext_substmt *info;
    long int v;
    long long int ll;
    unsigned long u;
    int i, j;

#define YIN_STORE_VALUE(TYPE, FROM, TO)           \
    *(TYPE **)TO = malloc(sizeof(TYPE));          \
    if (!*(TYPE **)TO) { LOGMEM(mod->ctx); goto error; }    \
    (**(TYPE **)TO) = (TYPE)FROM;

#define YIN_EXTCOMPLEX_GETPLACE(STMT, TYPE)                                          \
    p = lys_ext_complex_get_substmt(STMT, ext, &info);                               \
    if (!p) {                                                                        \
        LOGVAL(mod->ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node->name, node->parent->name); \
        goto error;                                                                  \
    }                                                                                \
    if (info->cardinality < LY_STMT_CARD_SOME && (*(TYPE*)p)) {                      \
        LOGVAL(mod->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, node->parent->name);     \
        goto error;                                                                  \
    }                                                                                \
    pp = NULL; i = 0;                                                                \
    if (info->cardinality >= LY_STMT_CARD_SOME) {                                    \
        /* there can be multiple instances */                                        \
        pp = p;                                                                      \
        if (!(*pp)) {                                                                \
            *pp = malloc(2 * sizeof(TYPE)); /* allocate initial array */             \
            LY_CHECK_ERR_GOTO(!*pp, LOGMEM(mod->ctx), error);                        \
        } else {                                                                     \
            for (i = 0; (*(TYPE**)pp)[i]; i++);                                      \
        }                                                                            \
        p = &(*(TYPE**)pp)[i];                                                       \
    }
#define YIN_EXTCOMPLEX_ENLARGE(TYPE)                         \
    if (pp) {                                                \
        /* enlarge the array */                              \
        reallocated = realloc(*pp, (i + 2) * sizeof(TYPE*)); \
        LY_CHECK_ERR_GOTO(!reallocated, LOGMEM(mod->ctx), error);      \
        *pp = reallocated;                                   \
        (*(TYPE**)pp)[i + 1] = 0;                            \
    }
#define YIN_EXTCOMPLEX_PARSE_SNODE(STMT, FUNC, ARGS...)                              \
    pp = (void**)yin_getplace_for_extcomplex_node(node, ext, STMT);                  \
    if (!pp) { goto error; }                                                         \
    if (!FUNC(mod, (struct lys_node*)ext, node, ##ARGS, LYS_PARSE_OPT_CFG_NOINHERIT, unres)) { goto error; }
#define YIN_EXTCOMPLEX_PARSE_RESTR(STMT)                                             \
    YIN_EXTCOMPLEX_GETPLACE(STMT, struct lys_restr*);                                \
    GETVAL(mod->ctx, value, node, "value");                                                    \
    *(struct lys_restr **)p = calloc(1, sizeof(struct lys_restr));                   \
    LY_CHECK_ERR_GOTO(!*(struct lys_restr **)p, LOGMEM(mod->ctx), error);            \
    (*(struct lys_restr **)p)->expr = lydict_insert(mod->ctx, value, 0);             \
    if (read_restr_substmt(mod, *(struct lys_restr **)p, node, unres)) {             \
        goto error;                                                                  \
    }                                                                                \
    YIN_EXTCOMPLEX_ENLARGE(struct lys_restr*);

    LY_TREE_FOR_SAFE(yin->child, next, node) {
        if (!node->ns) {
            /* garbage */
        } else if (node->ns == yin->ns && (ext->flags & LYS_YINELEM) && ly_strequal(node->name, ext->def->argument, 1)) {
            /* we have the extension's argument */
            if (ext->arg_value) {
                LOGVAL(mod->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, yin->name);
                goto error;
            }
            ext->arg_value = node->content;
            node->content = NULL;
        } else if (strcmp(node->ns->value, LY_NSYIN)) {
            /* extension */
            if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, LYEXT_SUBSTMT_SELF, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "description")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_DESCRIPTION, 1, "text", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "reference")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_REFERENCE, 1, "text", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "units")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_UNITS, 0, "name", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "type")) {
            type = (struct lys_type **)yin_getplace_for_extcomplex_struct(node, ext, LY_STMT_TYPE);
            if (!type) {
                goto error;
            }
            /* allocate type structure */
            (*type) = calloc(1, sizeof **type);
            LY_CHECK_ERR_GOTO(!*type, LOGMEM(mod->ctx), error);

            /* HACK for unres */
            lyxml_unlink(mod->ctx, node);
            (*type)->der = (struct lys_tpdf *)node;
            (*type)->parent = (struct lys_tpdf *)ext;

            if (unres_schema_add_node(mod, unres, *type, UNRES_TYPE_DER_EXT, NULL) == -1) {
                (*type)->der = NULL;
                goto error;
            }
            continue; /* skip lyxml_free() */
        } else if (!strcmp(node->name, "typedef")) {
            pp = yin_getplace_for_extcomplex_struct(node, ext, LY_STMT_TYPEDEF);
            if (!pp) {
                goto error;
            }
            /* allocate typedef structure */
            (*pp) = calloc(1, sizeof(struct lys_tpdf));
            LY_CHECK_ERR_GOTO(!*pp, LOGMEM(mod->ctx), error);

            if (fill_yin_typedef(mod, (struct lys_node *)ext, node, *((struct lys_tpdf **)pp), unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "if-feature")) {
            pp = yin_getplace_for_extcomplex_struct(node, ext, LY_STMT_IFFEATURE);
            if (!pp) {
                goto error;
            }
            /* allocate iffeature structure */
            (*pp) = calloc(1, sizeof(struct lys_iffeature));
            LY_CHECK_ERR_GOTO(!*pp, LOGMEM(mod->ctx), error);

            if (fill_yin_iffeature((struct lys_node *)ext, 0, node, *((struct lys_iffeature **)pp), unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "status")) {
            p = yin_getplace_for_extcomplex_flags(node, ext, LY_STMT_STATUS, LYS_STATUS_MASK);
            if (!p) {
                goto error;
            }

            GETVAL(mod->ctx, value, node, "value");
            if (!strcmp(value, "current")) {
                *(uint16_t*)p |= LYS_STATUS_CURR;
            } else if (!strcmp(value, "deprecated")) {
                *(uint16_t*)p |= LYS_STATUS_DEPRC;
            } else if (!strcmp(value, "obsolete")) {
                *(uint16_t*)p |= LYS_STATUS_OBSLT;
            } else {
                LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, node->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, LYEXT_SUBSTMT_STATUS, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "config")) {
            if (yin_parse_extcomplex_flag(mod, node, ext, LY_STMT_MANDATORY, "true", "false", LYS_CONFIG_MASK,
                                          LYS_CONFIG_W | LYS_CONFIG_SET, LYS_CONFIG_R | LYS_CONFIG_SET, unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "argument")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_ARGUMENT, 0, "name", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "default")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_DEFAULT, 0, "value", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "mandatory")) {
            if (yin_parse_extcomplex_flag(mod, node, ext, LY_STMT_MANDATORY,
                                         "true", "false", LYS_MAND_MASK, LYS_MAND_TRUE, LYS_MAND_FALSE, unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "error-app-tag")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_ERRTAG, 0, "value", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "error-message")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_ERRMSG, 1, "value", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "prefix")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_PREFIX, 0, "value", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "namespace")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_NAMESPACE, 0, "uri", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "presence")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_PRESENCE, 0, "value", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "revision-date")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_REVISIONDATE, 0, "date", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "key")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_KEY, 0, "value", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "base")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_BASE, 0, "name", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "ordered-by")) {
            if (yin_parse_extcomplex_flag(mod, node, ext, LY_STMT_ORDEREDBY,
                                          "user", "system", LYS_USERORDERED, LYS_USERORDERED, 0, unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "belongs-to")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_BELONGSTO, 0, "module", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "contact")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_CONTACT, 1, "text", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "organization")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_ORGANIZATION, 1, "text", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "path")) {
            if (yin_parse_extcomplex_str(mod, node, ext, LY_STMT_PATH, 0, "value", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "require-instance")) {
            if (yin_parse_extcomplex_bool(mod, node, ext, LY_STMT_REQINSTANCE, "true", "false", unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "modifier")) {
            if (yin_parse_extcomplex_bool(mod, node, ext, LY_STMT_MODIFIER, "invert-match", NULL, unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "fraction-digits")) {
            YIN_EXTCOMPLEX_GETPLACE(LY_STMT_DIGITS, uint8_t);

            GETVAL(mod->ctx, value, node, "value");
            v = strtol(value, NULL, 10);

            /* range check */
            if (v < 1 || v > 18) {
                LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, node->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, LYEXT_SUBSTMT_STATUS, i, unres)) {
                goto error;
            }

            /* store the value */
            (*(uint8_t *)p) = (uint8_t)v;

            YIN_EXTCOMPLEX_ENLARGE(uint8_t);
        } else if (!strcmp(node->name, "max-elements")) {
            YIN_EXTCOMPLEX_GETPLACE(LY_STMT_MAX, uint32_t*);

            GETVAL(mod->ctx, value, node, "value");
            while (isspace(value[0])) {
                value++;
            }

            if (!strcmp(value, "unbounded")) {
                u = 0;
            } else {
                /* convert it to uint32_t */
                errno = 0; endptr = NULL;
                u = strtoul(value, &endptr, 10);
                if (*endptr || value[0] == '-' || errno || u == 0 || u > UINT32_MAX) {
                    LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, node->name);
                    goto error;
                }
            }

            if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, LYEXT_SUBSTMT_MAX, i, unres)) {
                goto error;
            }

            /* store the value */
            YIN_STORE_VALUE(uint32_t, u, p)

            YIN_EXTCOMPLEX_ENLARGE(uint32_t*);
        } else if (!strcmp(node->name, "min-elements")) {
            YIN_EXTCOMPLEX_GETPLACE(LY_STMT_MIN, uint32_t*);

            GETVAL(mod->ctx, value, node, "value");
            while (isspace(value[0])) {
                value++;
            }

            /* convert it to uint32_t */
            errno = 0;
            endptr = NULL;
            u = strtoul(value, &endptr, 10);
            if (*endptr || value[0] == '-' || errno || u > UINT32_MAX) {
                LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, node->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, LYEXT_SUBSTMT_MAX, i, unres)) {
                goto error;
            }

            /* store the value */
            YIN_STORE_VALUE(uint32_t, u, p)

            YIN_EXTCOMPLEX_ENLARGE(uint32_t*);
        } else if (!strcmp(node->name, "value")) {
            YIN_EXTCOMPLEX_GETPLACE(LY_STMT_VALUE, int32_t*);

            GETVAL(mod->ctx, value, node, "value");
            while (isspace(value[0])) {
                value++;
            }

            /* convert it to int32_t */
            ll = strtoll(value, NULL, 10);

            /* range check */
            if (ll < INT32_MIN || ll > INT32_MAX) {
                LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, node->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, LYEXT_SUBSTMT_VALUE, i, unres)) {
                goto error;
            }

            /* store the value */
            YIN_STORE_VALUE(int32_t, ll, p)

            YIN_EXTCOMPLEX_ENLARGE(int32_t*);
        } else if (!strcmp(node->name, "position")) {
            YIN_EXTCOMPLEX_GETPLACE(LY_STMT_POSITION, uint32_t*);

            GETVAL(mod->ctx, value, node, "value");
            ll = strtoll(value, NULL, 10);

            /* range check */
            if (ll < 0 || ll > UINT32_MAX) {
                LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, node->name);
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, LYEXT_SUBSTMT_POSITION, i, unres)) {
                goto error;
            }

            /* store the value */
            YIN_STORE_VALUE(uint32_t, ll, p)

            YIN_EXTCOMPLEX_ENLARGE(uint32_t*);
        } else if (!strcmp(node->name, "module")) {
            pp = yin_getplace_for_extcomplex_struct(node, ext, LY_STMT_MODULE);
            if (!pp) {
                goto error;
            }

            *(struct lys_module **)pp = yin_read_module_(mod->ctx, node, NULL, mod->implemented);
            if (!(*pp)) {
                goto error;
            }
        } else if (!strcmp(node->name, "when")) {
            YIN_EXTCOMPLEX_GETPLACE(LY_STMT_WHEN, struct lys_when*);

            *(struct lys_when**)p = read_yin_when(mod, node, unres);
            if (!*(struct lys_when**)p) {
                goto error;
            }

            YIN_EXTCOMPLEX_ENLARGE(struct lys_when*);
        } else if (!strcmp(node->name, "revision")) {
            YIN_EXTCOMPLEX_GETPLACE(LY_STMT_REVISION, struct lys_revision*);

            *(struct lys_revision**)p = calloc(1, sizeof(struct lys_revision));
            LY_CHECK_ERR_GOTO(!*(struct lys_revision**)p, LOGMEM(mod->ctx), error);
            if (fill_yin_revision(mod, node, *(struct lys_revision**)p, unres)) {
                goto error;
            }

            /* check uniqueness of the revision dates - not required by RFC */
            if (pp) {
                for (j = 0; j < i; j++) {
                    if (!strcmp((*(struct lys_revision***)pp)[j]->date, (*(struct lys_revision**)p)->date)) {
                        LOGWRN(mod->ctx, "Module's revisions are not unique (%s).", (*(struct lys_revision**)p)->date);
                    }
                }
            }

            YIN_EXTCOMPLEX_ENLARGE(struct lys_revision*);
        } else if (!strcmp(node->name, "unique")) {
            YIN_EXTCOMPLEX_GETPLACE(LY_STMT_UNIQUE, struct lys_unique*);

            *(struct lys_unique**)p = calloc(1, sizeof(struct lys_unique));
            LY_CHECK_ERR_GOTO(!*(struct lys_unique**)p, LOGMEM(mod->ctx), error);
            if (fill_yin_unique(mod, (struct lys_node*)ext, node, *(struct lys_unique**)p, unres)) {
                goto error;
            }

            if (lyp_yin_parse_subnode_ext(mod, ext, LYEXT_PAR_EXTINST, node, LYEXT_SUBSTMT_UNIQUE, i, unres)) {
                goto error;
            }
            YIN_EXTCOMPLEX_ENLARGE(struct lys_unique*);
        } else if (!strcmp(node->name, "action")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_ACTION, read_yin_rpc_action);
        } else if (!strcmp(node->name, "anydata")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_ANYDATA, read_yin_anydata, LYS_ANYDATA);
        } else if (!strcmp(node->name, "anyxml")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_ANYXML, read_yin_anydata, LYS_ANYXML);
        } else if (!strcmp(node->name, "case")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_CASE, read_yin_case);
        } else if (!strcmp(node->name, "choice")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_CHOICE, read_yin_choice);
        } else if (!strcmp(node->name, "container")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_CONTAINER, read_yin_container);
        } else if (!strcmp(node->name, "grouping")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_GROUPING, read_yin_grouping);
        } else if (!strcmp(node->name, "output")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_OUTPUT, read_yin_input_output);
        } else if (!strcmp(node->name, "input")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_INPUT, read_yin_input_output);
        } else if (!strcmp(node->name, "leaf")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_LEAF, read_yin_leaf);
        } else if (!strcmp(node->name, "leaf-list")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_LEAFLIST, read_yin_leaflist);
        } else if (!strcmp(node->name, "list")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_LIST, read_yin_list);
        } else if (!strcmp(node->name, "notification")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_NOTIFICATION, read_yin_notif);
        } else if (!strcmp(node->name, "uses")) {
            YIN_EXTCOMPLEX_PARSE_SNODE(LY_STMT_USES, read_yin_uses);
        } else if (!strcmp(node->name, "length")) {
            YIN_EXTCOMPLEX_PARSE_RESTR(LY_STMT_LENGTH);
        } else if (!strcmp(node->name, "must")) {
            pp = yin_getplace_for_extcomplex_struct(node, ext, LY_STMT_MUST);
            if (!pp) {
                goto error;
            }
            /* allocate structure for must */
            (*pp) = calloc(1, sizeof(struct lys_restr));
            LY_CHECK_ERR_GOTO(!*pp, LOGMEM(mod->ctx), error);

            if (fill_yin_must(mod, node, *((struct lys_restr **)pp), unres)) {
                goto error;
            }
        } else if (!strcmp(node->name, "pattern")) {
            YIN_EXTCOMPLEX_GETPLACE(LY_STMT_PATTERN, struct lys_restr*);
            GETVAL(mod->ctx, value, node, "value");
            if (lyp_check_pattern(mod->ctx, value, NULL)) {
                goto error;
            }

            *(struct lys_restr **)p = calloc(1, sizeof(struct lys_restr));
            LY_CHECK_ERR_GOTO(!*(struct lys_restr **)p, LOGMEM(mod->ctx), error);

            modifier = 0x06; /* ACK */
            if (mod->version >= 2) {
                name = NULL;
                LY_TREE_FOR(node->child, child) {
                    if (child->ns && !strcmp(child->ns->value, LY_NSYIN) && !strcmp(child->name, "modifier")) {
                        if (name) {
                            LOGVAL(mod->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "modifier", node->name);
                            goto error;
                        }

                        GETVAL(mod->ctx, name, child, "value");
                        if (!strcmp(name, "invert-match")) {
                            modifier = 0x15; /* NACK */
                        } else {
                            LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, name, "modifier");
                            goto error;
                        }
                        /* get extensions of the modifier */
                        if (lyp_yin_parse_subnode_ext(mod, *(struct lys_restr **)p, LYEXT_PAR_RESTR, child,
                                                      LYEXT_SUBSTMT_MODIFIER, 0, unres)) {
                            goto error;
                        }
                    }
                }
            }

            /* store the value: modifier byte + value + terminating NULL byte */
            (*(struct lys_restr **)p)->expr = malloc((strlen(value) + 2) * sizeof(char));
            LY_CHECK_ERR_GOTO(!(*(struct lys_restr **)p)->expr, LOGMEM(mod->ctx), error);
            ((char *)(*(struct lys_restr **)p)->expr)[0] = modifier;
            strcpy(&((char *)(*(struct lys_restr **)p)->expr)[1], value);
            lydict_insert_zc(mod->ctx, (char *)(*(struct lys_restr **)p)->expr);

            /* get possible sub-statements */
            if (read_restr_substmt(mod, *(struct lys_restr **)p, node, unres)) {
                goto error;
            }

            YIN_EXTCOMPLEX_ENLARGE(struct lys_restr*);
        } else if (!strcmp(node->name, "range")) {
            YIN_EXTCOMPLEX_PARSE_RESTR(LY_STMT_RANGE);
        } else {
            LOGERR(mod->ctx, ly_errno, "Extension's substatement \"%s\" not supported.", node->name);
        }
        lyxml_free(mod->ctx, node);
    }

    if (ext->substmt && lyp_mand_check_ext(ext, yin->name)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}